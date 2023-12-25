#include "sph.kernels.glsl" 

vec4 solveParticleCollision(uint i){
	int n = 0;
	vec4 deltaPosition = vec4(0);
	OVERNNS
		//if(particles[j].phase == GRANULAR) continue; //Ignore particle that are too far
		highp float l0 = 2.0*particleRadius;
		highp float l = distance(particles[i].new_position, particles[j].new_position); // Calculate the current length
		// Calculate the constraint violation
		highp float C = l - l0;

		if( C >= 0) continue;
		// Calculate the gradient of the constraint function
		vec4 gradC = normalize(particles[i].new_position - particles[j].new_position);

		// Calculate the effective mass matrix
		highp float inverseMass = (1.0/particles[i].mass) + (1.0/particles[j].mass);


		highp float compliance = 1.0f / stiffness;//0.000002; // Compliance of the constraints

		// Calculate the Lagrange multiplier delta (note : gradient dot product = 1 because normalized)
		//highp float deltaLambda = -C / ( inverseMass + (compliance/(dt*dt)));
				
				
		highp float complianceSc = compliance/(dt*dt);
		//highp float deltaLambda = -C/ (inverseMass + complianceSc);
		highp float deltaLambda = (- C - particles[i].lambda * complianceSc) / ( dot(gradC, gradC) * inverseMass + complianceSc);


		// Calculate and apply the position corrections
		deltaPosition += gradC * deltaLambda * particles[i].mass;
		n++;
		particles[i].lambda += deltaLambda;

	OVERNNS_END

	if(n > 1) return deltaPosition/float(n);
	else return deltaPosition;
}


vec4 solveFriction(uint i){
	int n = 0;
	vec4 deltaPosition = vec4(0);
	OVERNNS
		if(particles[j].phase != GRANULAR) continue; //Ignore particle that are too far
		highp float l0 = 2.0*particleRadius;
		highp float l = distance(particles[i].pposition, particles[j].pposition); // Calculate the current length
		// Calculate the constraint violation
		highp float C = l - l0;

		if( C >= 0 || abs(C) < 1e-06) continue;
		highp float inverseMass = (1.0/particles[i].mass) + (1.0/particles[j].mass);
		deltaPosition += (particles[i].mass/inverseMass) * ((particles[i].new_position - particles[i].position) - (particles[j].new_position - particles[j].position));
		n++;
	OVERNNS_END
	if(n > 1) return deltaPosition/float(n);
	else return deltaPosition;
}

vec4 solveDistanceConstraint(uint i){
	int n = 0;
	vec4 deltaPosition = vec4(0);
	OVERNNS
		//if(particles[j].phase != SOLID) continue; //Ignore particle that are too far
		float l0 = distance(particles[i].initial_position, particles[j].initial_position);
		if(l0 >= H) continue; //Ignore particle that are too far

		float l = distance(particles[i].pposition, particles[j].pposition); // Calculate the current length
		// Calculate the constraint violation
		float C = l - l0;

		//if(abs(C) < 1e-06) continue;
		// Calculate the gradient of the constraint function
		vec4 gradC = normalize(particles[i].pposition - particles[j].pposition);

		// Calculate the effective mass matrix
		float inverseMass = (1.0/particles[i].mass) + (1.0/particles[j].mass);

		float compliance = 1.0f / stiffness;//0.000002; // Compliance of the constraints
		float damping = -0.000000001; // Compliance of the constraints
		float complianceSc = compliance/(dt*dt);
		float dampingSc = damping*(dt*dt);

		float gamma = (complianceSc * dampingSc)/dt;
		float deltaLambda = (-C - gamma )/ ((1+gamma) * inverseMass + complianceSc);
		//highp float deltaLambda = -C / ( inverseMass + (complianceSc));



		// Calculate and apply the position corrections
		deltaPosition += gradC * deltaLambda * particles[i].mass;
		n++;
		particles[i].lambda += deltaLambda;

	OVERNNS_END
	if(n > 1) return deltaPosition/float(n);
	else return deltaPosition;
}

vec4 solveFloorCollision(uint i){
	vec4 correctedPosition = particles[i].new_position;
	for (int i = 0; i < 3; i++){
		if (correctedPosition[i] - particleRadius < boundaryMin[i] )
		{
			correctedPosition[i] = (boundaryMin[i]) + particleRadius;
			//velocity[i] *= boundaryDamping;

		}
		else if (correctedPosition[i] + particleRadius > boundaryMax[i])
		{
			correctedPosition[i] =  boundaryMax[i] - particleRadius;
			//velocity[i] *= boundaryDamping;
		}
	}

	return (correctedPosition - particles[i].new_position);
}


void clampPosition(inout vec4 position){
	vec4 correctedPosition = position;
	for (int i = 0; i < 3; i++){
		if (correctedPosition[i] - particleRadius <= boundaryMin[i] )
		{
			correctedPosition[i] = (boundaryMin[i]) + particleRadius;
			//velocity[i] *= boundaryDamping;

		}
		else if (correctedPosition[i] + particleRadius >= boundaryMax[i])
		{
			correctedPosition[i] =  boundaryMax[i] - particleRadius;
			//velocity[i] *= boundaryDamping;
		}
	}

	position = correctedPosition;
}


// --------- Position based fluid --------------
vec4 solveViscosity(uint i) {
	vec4 smoothedVelocity = vec4(0);
	OVERNNS
		Particle pj = particles[j];
		if(pj.phase == FLUID){
			vec4 rij = particles[i].new_position - particles[j].new_position;
			vec4 vij = particles[i].velocity - particles[j].velocity;

			float Wij = Wvisc(rij);
			smoothedVelocity += (pj.mass / pj.density) * vij * Wij ;

		}
	OVERNNS_END

	return artificialViscosity * smoothedVelocity;
}

float computeDensity(uint i) {
	float density = particleMass * poly6(vec4(0.0));
	Particle pi = particles[i];

	OVERNNS
		Particle pj = particles[j];
		if( pj.phase != FLUID) continue;
		vec4 diff = pi.position - pj.position;
		density += pj.mass*poly6(diff); //Fluid, solid or boundary (mass and pseudo mass stored at the same location : See init.comp)
	OVERNNS_END
	return  density;
}

// Introduce the viscosity constraint and its gradient
float densityConstraint(float density) {
    return (density * INV_REST_DENSITY) - 1.0f;
}

vec4 densityConstraintGradient(vec4 pi, vec4 pj) {
	vec4 diff = pi - pj;
	if (length(diff.xyz) > H) return vec4(0.0);//neighbor too far
	return spiky_grad(diff);
}

// Introduce the viscosity constraint and its gradient
float viscosityConstraint(vec4 p_i, vec4 p_j, float d_ij) {
    return d_ij - length(p_i - p_j) - artificialViscosity * d_ij;
}

vec4 viscosityConstraintGradient(vec4 p_i, vec4 p_j) {
    vec4 p_ij = p_i - p_j;
    return normalize(p_ij);
}



float computeLambda(uint i) {
	Particle pi = particles[i];
    if(pi.density == REST_DENSITY) return 0.0;

    float rho0 = REST_DENSITY;
    float sumGradCj = 0;
    vec4 gradCi = vec4(0);
    float density = 0.0;
    
    OVERNNS_I
		Particle pj = particles[j];
        if(pj.phase != FLUID) continue;
  
		vec4 dist = pi.pposition - pj.pposition;
		vec4 gradCj = densityConstraintGradient(pi.pposition, pj.pposition);
		if (j == i) {
			gradCi += gradCj;
		}
		sumGradCj += dot(gradCj, gradCj);
		density += pj.mass * poly6(dist);
	OVERNNS_END

	particles[i].density = density;
    float C = max((density / rho0) - 1, 0.0);
    return -C / (sumGradCj + 1e-6);
}


//Compute position deltas w neigbor
vec4 computePositionDelta(uint i) {
	vec4 posDelta = vec4(0.0f);
	float rho0 = REST_DENSITY;
	Particle pi = particles[i];
	OVERNNS
		Particle pj = particles[j];
		if( pj.phase != FLUID) continue;
		  vec4 dist = pi.pposition - pj.pposition;
          float sCorr = -(pressureMultiplier) * pow(poly6(dist) / poly6(vec4(0.3*H, 0,0,0)), 4);
          vec4 dp_ij = spiky_grad(dist) * ((pi.lambda + pj.lambda  + sCorr ) / rho0);
          posDelta += dp_ij;
	OVERNNS_END
	return posDelta;
}
