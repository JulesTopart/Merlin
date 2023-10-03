class Particle 
	{
		constructor (x, y) 
		{
			this.x = x;				// position
			this.y = y;	
			this.xOld = x;			// last position
			this.yOld = y;	
			this.vx = 0;			// velocity
			this.vy = 0;	
			this.ax = 0;			// acceleration
			this.ay = 0;	
			this.deltaX_x = 0;		// position correction
			this.deltaX_y = 0;				
			this.density = 0;		// density
			this.lambda = 0;		// Lagrange multiplier
			this.mass = 0;			// mass
			this.neighbors = [];	// list of neighbors
		}
	}
	
	class BoundaryParticle 
	{
		constructor (x, y) 
		{
			this.x = x;				// position
			this.y = y;
			this.psi = 0.5;			// pseudo mass
			this.neighbors = [];	// list of neighbors
		}
	}
	
	class GridCell 
	{
		constructor () 
		{
			this.timeStamp = -2.0;
			this.particles = [];	
		}
	}
	
	class Simulation
	{
		constructor(width, height)
		{
			this.particles = [];
			this.boundaryParticles = [];
			this.particleRadius = 0.025;
			this.supportRadius = 4.0*this.particleRadius;		// support radius is 4x particle radius
			this.density0 = 1000.0;								// rest density of water
			this.viscosity = 0.01;
			this.diam = 2.0 * this.particleRadius;
			this.mass = this.diam*this.diam*this.density0;		// mass = area * rest density
			this.timeStepSize = 0.005;
			this.maxIterations = 100;
			this.time = 0;
			this.maxError = 0.01;
			this.iter = 0;
			this.gridMap = new Array(100000);
			for (let i = 0; i < 100000; i++) 
				this.gridMap[i] = new GridCell();
			
			// constants for kernel computation
			this.kernel_k = 40.0 / (7.0 * (Math.PI*this.supportRadius*this.supportRadius));	  
			this.kernel_l = 240.0 / (7.0 * (Math.PI*this.supportRadius*this.supportRadius));
			this.kernel_0 = this.cubicKernel2D(0);
					
			this.width = width;
			this.height = height;
			this.gravity = -9.81;
			this.numFluidParticles = 0;
			this.numParticles = 0;
			
			this.init();
		}
		
		// initialize scene: generate a block of water particles and
		// a box of boundary particles around
		init()
		{			
			// create particles  
			let i;
			let j;
			let w = this.width;
			let h = this.height;
			let bw = 3*w;
			let bh = 3*h;
			
			// generate a block of fluid particles 
			for (i = 0; i < h; i++) 
			{
				for (j = 0; j < w; j++) 
				{
					this.particles.push(new Particle(
						-0.5*bw*this.diam + j*this.diam + this.diam + this.particleRadius, 
						i*this.diam + this.diam + this.particleRadius));
				}
			}
			this.numFluidParticles = this.particles.length;
	
			// generate a box of boundary particles
			for (j = 0; j < bw; j++) 
			{
				// bottom
				this.particles.push(new BoundaryParticle(-0.5*bw*this.diam + j*this.diam, 0));
				// top
				this.particles.push(new BoundaryParticle(-0.5*bw*this.diam + j*this.diam, bh*this.diam));
			}
			
			for (j = 1; j < bh; j++) 
			{
				// left
				this.particles.push(new BoundaryParticle(-0.5*bw*this.diam, j*this.diam));
				// right
				this.particles.push(new BoundaryParticle(-0.5*bw*this.diam + (bw-1)*this.diam, j*this.diam));
			}
			this.numParticles = this.particles.length;
			
			// compute pseudo mass psi for boundary particles
			let boundary = [];
			for (i = this.numFluidParticles; i < this.numParticles; i++) 
			{
				// temporary copy of boundary positions 
				boundary.push(this.particles[i]);
			}
			
			// set timestamps to -1.0 since the neighborhood search is performed in a 
			// precomputation step
			this.time = -1.0;
			this.neighborHoodSearch(boundary, boundary.length, boundary.length);
			this.time = 0.0;
			
			// pseudo mass is computed as (rest density) / sum_j W_ij
			for (i = this.numFluidParticles; i < this.numParticles; i++) 
			{
				let index = i-this.numFluidParticles;
				let delta = this.kernel_0;
				let nl = boundary[index].neighbors.length;					
				for(j=0; j < nl; j++)
				{
					let nj = boundary[index].neighbors[j]+this.numFluidParticles;
					let xi_xj_x = this.particles[i].x - this.particles[nj].x;
					let xi_xj_y = this.particles[i].y - this.particles[nj].y;
					delta += this.cubicKernel2D(this.norm(xi_xj_x, xi_xj_y));
				}
				this.particles[i].psi = 1.0*this.density0 * 1.0/delta;
			}
		}
		
		// compute the norm of a vector (x,y)
		norm(x, y)
		{
			return Math.sqrt(x*x + y*y);
		}
		
		// compute the squared norm of a vector (x,y)
		squardNorm(x, y)
		{
			return (x*x + y*y);
		}
		
		// Cubic spline kernel 2D
		cubicKernel2D(r)
		{
			let res = 0.0;
			let q = r / this.supportRadius;
			if (q <= 1.0)
			{
				let q2 = q*q;
				let q3 = q2*q;
				if (q <= 0.5)
					res = this.kernel_k * (6.0*q3 - 6.0*q2 + 1.0);
				else
					res = this.kernel_k * (2.0*Math.pow(1.0 - q, 3));
			}
			return res;
		}
		
		// Gradient of cubic spline kernel 2D
		cubicKernel2D_Gradient(rx, ry)
		{
			let res = [0,0];
			let rl = this.norm(rx, ry);
			let q = rl / this.supportRadius;
			if (q <= 1.0)
			{
				if (rl > 1.0e-6)
				{
					let gradq_x = rx * (1.0 / (rl*this.supportRadius));
					let gradq_y = ry * (1.0 / (rl*this.supportRadius));
					if (q <= 0.5)
					{
						res[0] = this.kernel_l*q*(3.0*q - 2.0) * gradq_x;
						res[1] = this.kernel_l*q*(3.0*q - 2.0) * gradq_y;
					}
					else
					{
						let factor = (1.0 - q) * (1.0 - q);
						res[0] = this.kernel_l*(-factor) * gradq_x;
						res[1] = this.kernel_l*(-factor) * gradq_y;
					}
				}
			}
			return res;
		}
		
		// hash function for spatial hashing (neighborhood search)
		hashFunction(x, y)
		{
			let p1 = 73856093 * x;
			let p2 = 19349663 * y;
			return Math.abs(p1 + p2) % 100000;
		}
		
		// search the neighbors of all fluid particles using spatial hashing
		neighborHoodSearch(p, numFluidParticles, numTotalParticles)
		{
			// fill grid with particles
			let invGridSize = 1.0/this.supportRadius;

			// fluid particles
			for (let i = 0; i < numTotalParticles; i++) 
			{
				let x = p[i].x;
				let y = p[i].y;
				
				// get position in grid
				let cellPos1 = Math.floor((x + 100.0) * invGridSize);
				let cellPos2 = Math.floor((y + 100.0) * invGridSize);
				
				// compute hash value
				let hash = this.hashFunction(cellPos1, cellPos2);
				
				// insert particle in hash map
				if (this.gridMap[hash].timeStamp == this.time)
					this.gridMap[hash].particles.push(i);
				else
				{
					this.gridMap[hash].particles = [i];
					this.gridMap[hash].timeStamp = this.time;
				}
			}
			
			
			// loop over all 9 neighboring cells
			let radius2 = this.supportRadius * this.supportRadius;
			for (let i = 0; i < numFluidParticles; i++) 
			{
				// reset neighbor list
				p[i].neighbors = [];
				let x = p[i].x;
				let y = p[i].y;
				let cellPos1 = Math.floor((x + 100.0) * invGridSize);
				let cellPos2 = Math.floor((y + 100.0) * invGridSize);
				for (let j = -1; j <= 1; j++)
				{
					for(let k = -1; k <= 1; k++)
					{
						// get hash value of neighboring cell
						let hash = this.hashFunction(cellPos1+j, cellPos2+k);
						if (this.gridMap[hash].timeStamp == this.time)
						{
							// if neighboring cell contains particles, get particle list
							let part = this.gridMap[hash].particles;
							
							// loop over particles in neighboring cell 
							// and add particles with a distance of less
							// than the support radius to neighbor list
							for (let l=0; l < part.length; l++)
							{
								let nIndex = part[l];
								if (nIndex != i)
								{
									let xn = p[nIndex].x;
									let yn = p[nIndex].y;
									let diffx = x-xn;
									let diffy = y-yn;
									let dist2 = diffx*diffx + diffy*diffy;
									
									// if distance to particle is < radius, add particle
									if (dist2 - radius2 < 1.0e-6)
										p[i].neighbors.push(nIndex);
								}
							}
						}
					}
				}
			}
		}
		
		// set all accelerations to (0, gravity) 		
		resetAccelerations()
		{
			let i;
			for	(i = 0; i < this.numFluidParticles; i++) 
			{
				let p = this.particles[i];
				p.ax = 0;
				p.ay = this.gravity;
			}
		}
		
		// perform time integration using the symplectic Euler method
		symplecticEuler()
		{
			let dt = this.timeStepSize;
			// symplectic Euler step
			let i;
			for (i = 0; i < this.numFluidParticles; i++) 
			{
				let p = this.particles[i];
					
				// store old position for velocity update after the position correction
				p.xOld = p.x;	
				p.yOld = p.y;
			
				// integrate velocity considering gravitational acceleration 
				p.vx = p.vx + dt * p.ax;
				p.vy = p.vy + dt * p.ay;
			
				// integrate position
				p.x = p.x + dt * p.vx;
				p.y = p.y + dt * p.vy;
			}
		}
		
		// simulation step
		simulationStep()
		{		
			// time integration (predict positions)
			this.symplecticEuler();
			
			// reset the accelerations of the particles
			this.resetAccelerations();
			
			// neighborhood search
			this.neighborHoodSearch(this.particles, this.numFluidParticles, this.numParticles);
			
			// perform pressure solve using PBF
			this.pressureSolve();
		
			// compute non-pressure forces
			this.computeViscosity();
							
			// update simulation time
			this.time = this.time + this.timeStepSize;
		}
		
		// solve density constraint using position-based fluids solver
		pressureSolve()
		{
			this.avg_density_err = 1000;
			this.iter = 0;
			while ((this.avg_density_err > this.maxError * 0.01 * this.density0) && (this.iter < this.maxIterations))	// max. error is given in percent
			{		
				this.computeLagrangeMultipliers();
				this.computePositionCorrections();
				
				// update positions
				for (let i = 0; i < this.numFluidParticles; i++) 
				{
					let p = this.particles[i];
					p.x = p.x + p.deltaX_x;
					p.y = p.y + p.deltaX_y;
				}	
				this.iter += 1;				
			}
			
			// update velocities
			for (let i = 0; i < this.numFluidParticles; i++) 
			{
				let p = this.particles[i];
				p.vx = 1.0/this.timeStepSize * (p.x - p.xOld);
				p.vy = 1.0/this.timeStepSize * (p.y - p.yOld);
			}

		}
		
		// compute Lagrange multipliers for density constraint
		computeLagrangeMultipliers()
		{
			for (let i = 0; i < this.numFluidParticles; i++) 
			{
				let p_i = this.particles[i];
				this.computeDensity(i);
							
				// evaluate density constraint function
				// and clamp to prevent particle clumping at surface
				let C = Math.max(p_i.density / this.density0 - 1.0, 0.0);

				this.avg_density_err += Math.max(p_i.density - this.density0, 0.0);

				if (C != 0.0)		// constraint violated
				{
					// Compute gradients dC/dx_j 
					let sum_grad_C2 = 0.0;
					let gradC_i_x = 0;
					let gradC_i_y = 0;
					
					let nl = p_i.neighbors.length;
					for(let j=0; j < nl; j++)
					{
						let nj = p_i.neighbors[j];
						let p_j = this.particles[nj];
						
						// compute grad W (xi-xj)
						let gradW = this.cubicKernel2D_Gradient(p_i.x - p_j.x, p_i.y - p_j.y);
						
						// Fluid
						if (nj < this.numFluidParticles)
						{
							let gradC_j_x = -this.mass/this.density0 * gradW[0];
							let gradC_j_y = -this.mass/this.density0 * gradW[1];
							sum_grad_C2 += this.squardNorm(gradC_j_x, gradC_j_y);
							gradC_i_x = gradC_i_x - gradC_j_x;
							gradC_i_y = gradC_i_y - gradC_j_y;
						}
						// Boundary
						else
						{						
							let gradC_j_x = -p_j.psi/this.density0 * gradW[0];
							let gradC_j_y = -p_j.psi/this.density0 * gradW[1];
							//sum_grad_C2 += this.squardNorm(gradC_j_x, gradC_j_y);
							gradC_i_x = gradC_i_x - gradC_j_x;
							gradC_i_y = gradC_i_y - gradC_j_y;
						}
					}

					sum_grad_C2 += this.squardNorm(gradC_i_x, gradC_i_y);

					// Compute lambda
					p_i.lambda = -C / (sum_grad_C2 + 1.0e-6);
				}
				else
					p_i.lambda = 0.0;
			}
			this.avg_density_err /= this.numFluidParticles;
		}
		
		// compute the position correction for the fluid particles
		// using the Lagrange multipliers
		computePositionCorrections()
		{
			for (let i = 0; i < this.numFluidParticles; i++) 
			{
				let p_i = this.particles[i];
				p_i.deltaX_x = 0;
				p_i.deltaX_y = 0;
				
				let nl = p_i.neighbors.length;
				for(let j=0; j < nl; j++)
				{
					let nj = p_i.neighbors[j];
					let p_j = this.particles[nj];
						
					// compute grad W (xi-xj)
					let gradW = this.cubicKernel2D_Gradient(p_i.x - p_j.x, p_i.y - p_j.y);
					
					// Fluid
					if (nj < this.numFluidParticles)
					{
						// Compute gradient dC/dx_j 
						let gradC_j_x = -this.mass/this.density0 * gradW[0];
						let gradC_j_y = -this.mass/this.density0 * gradW[1];
						p_i.deltaX_x = p_i.deltaX_x - (p_i.lambda + p_j.lambda) * gradC_j_x;
						p_i.deltaX_y = p_i.deltaX_y - (p_i.lambda + p_j.lambda) * gradC_j_y;
					}
					// Boundary
					else
					{						
						// Compute gradient dC/dx_j 
						let gradC_j_x = -p_j.psi/this.density0 * gradW[0];
						let gradC_j_y = -p_j.psi/this.density0 * gradW[1];
						p_i.deltaX_x = p_i.deltaX_x - p_i.lambda * gradC_j_x;
						p_i.deltaX_y = p_i.deltaX_y - p_i.lambda * gradC_j_y;
					}
				}
			}
		}
		
		// compute the density of particle i using the SPH formulation
		computeDensity(i)
		{
			let p_i = this.particles[i];
			
			// consider particle i
			p_i.density = this.mass * this.kernel_0;
		
			let nl = p_i.neighbors.length;
			for(let j=0; j < nl; j++)
			{
				let nj = p_i.neighbors[j];
				let p_j = this.particles[nj];
				
				// compute W (xi-xj)
				let Wij = this.cubicKernel2D(this.norm(p_i.x - p_j.x, p_i.y - p_j.y));
				
				// Fluid
				if (nj < this.numFluidParticles)
					p_i.density += this.mass * Wij;	
				else			// Boundary
					p_i.density += p_j.psi * Wij;
			}
		}
		
		// compute the viscosity forces (XSPH) for all particles 
		computeViscosity()
		{
			for (let i = 0; i < this.numFluidParticles; i++) 
			{		
				let p_i = this.particles[i];
				let nl = p_i.neighbors.length;
				for(let j=0; j < nl; j++)
				{
					let nj = p_i.neighbors[j];
					let p_j = this.particles[nj];
					
					// Fluid
					if (nj < this.numFluidParticles)
					{
						let vi_vj_x = p_i.vx - p_j.vx;	
						let vi_vj_y = p_i.vy - p_j.vy;
						// compute W (xi-xj)
						let Wij = this.cubicKernel2D(this.norm(p_i.x - p_j.x, p_i.y - p_j.y));

						let factor = this.mass/p_j.density * 1.0/this.timeStepSize * this.viscosity*Wij;
						p_i.ax -= factor * vi_vj_x;
						p_i.ay -= factor * vi_vj_y;
					}
				}
			}
		}
	}
	
	class GUI
	{
		constructor()
		{
			this.canvas = document.getElementById("simCanvas");
			this.c = this.canvas.getContext("2d");
			this.requestID = -1;
			
			this.timeSum = 0.0;
			this.counter = 0;
			this.pause = false;
			this.origin = { x : this.canvas.width / 2, y : this.canvas.height/2+200};
			this.zoom = 100;
			this.selectedParticle = -1;
			
			// register mouse event listeners (zoom/selection)
			this.canvas.addEventListener("mousedown", this.mouseDown.bind(this), false);
			this.canvas.addEventListener("mousemove", this.mouseMove.bind(this), false);
			this.canvas.addEventListener("mouseup", this.mouseUp.bind(this), false);	
			this.canvas.addEventListener("wheel", this.wheel.bind(this), false);		
		}
		
		// set simulation parameters from GUI and start mainLoop
		restart()
		{
			window.cancelAnimationFrame(this.requestID);
			let w = parseInt(document.getElementById('widthInput').value);
			let h = parseInt(document.getElementById('heightInput').value);
			delete this.sim;
			this.sim = new Simulation(w, h);
			
			this.timeSum = 0.0;
			this.counter = 0;

			this.sim.maxIterations = parseInt(document.getElementById('iterationsInput').value);
			this.sim.viscosity = parseFloat(document.getElementById('viscosityInput').value);
			this.sim.gravity = parseFloat(document.getElementById('gravityInput').value);
			this.sim.timeStepSize = parseFloat(document.getElementById('timeStepSizeInput').value);
			this.sim.maxError = parseFloat(document.getElementById('maxErrorInput').value);
			
			document.getElementById("numParticles").innerHTML = this.sim.particles.length;
			this.mainLoop();
		}
		
		// render scene
		draw()
		{
			this.c.clearRect(0, 0, this.canvas.width, this.canvas.height);
					
			// draw fluid particles as circles
			for (let i = 0; i < this.sim.numFluidParticles; i++) 
			{
				let p = this.sim.particles[i];
				let r = this.sim.particleRadius;
				if (i == this.selectedParticle)
				{
					// draw selected particle in red with larger radius
					this.c.fillStyle = "#FF0000";	
					r = 3*r;
				}
				else 
				{
					// color-coding of velocity
					let v = this.sim.norm(p.vx, p.vy);
					v = Math.min(v, 8.0);
					this.c.fillStyle='hsl(225,100%,' + (25.0+50.0*v/8.0) + '%)';
				}
				let px = this.origin.x + p.x * this.zoom;
				let py = this.origin.y - p.y * this.zoom;
				
				this.c.beginPath();			
				this.c.arc(px, py, r * this.zoom, 0, Math.PI*2, true); 
				this.c.closePath();
				this.c.fill();
			}
			
			// draw boundary particles as circles
			for (let i = this.sim.numFluidParticles; i < this.sim.numParticles; i++) 
			{
				let p = this.sim.particles[i];
				let r = this.sim.particleRadius;
				this.c.fillStyle = "#888888";	
				let px = this.origin.x + p.x * this.zoom;
				let py = this.origin.y - p.y * this.zoom;
				
				this.c.beginPath();			
				this.c.arc(px, py, r * this.zoom, 0, Math.PI*2, true); 
				this.c.closePath();
				this.c.fill();
			}
		}
		
		mainLoop()
		{
			// perform multiple sim steps per render step
			for (let i=0; i < 8; i++)
			{
				let t0 = performance.now();
				
				this.sim.simulationStep();

				let t1 = performance.now();
				this.timeSum += t1 - t0;
				this.counter += 1;
					
				if (this.counter % 50 == 0)				
				{
					this.timeSum /= this.counter;
					document.getElementById("timePerStep").innerHTML = this.timeSum.toFixed(2);		
					this.timeSum = 0.0;
					this.counter = 0;
				}	
				document.getElementById("time").innerHTML = this.sim.time.toFixed(2);	
				document.getElementById("iterations").innerHTML = this.sim.iter;	
			}			

			this.draw();
			if (!this.pause)
				this.requestID = window.requestAnimationFrame(this.mainLoop.bind(this));		
		}
		
		doPause()
		{
			this.pause = !this.pause;
			if (!this.pause)
				this.mainLoop();
		}
		
		mouseDown(event)
		{
			// left mouse button down
			if (event.which == 1)
			{
				let mousePos = this.getMousePos(this.canvas, event);
				for (let i = 0; i < this.sim.particles.length; i++) 
				{
					let p = this.sim.particles[i];
					let px = this.origin.x + p.x * this.zoom;
					let py = this.origin.y - p.y * this.zoom;
					let dx = px - mousePos.x
					let dy = py - mousePos.y
					let dist2 = Math.sqrt(dx * dx + dy * dy)
					if (dist2 < 10)
					{
						this.selectedParticle = i;
						break;
					}				
				}			
			}
		}
		
		getMousePos(canvas, event) 
		{
			let rect = canvas.getBoundingClientRect();
			return {
				x: event.clientX - rect.left,
				y: event.clientY - rect.top
			};
		}
	
		mouseMove(event)
		{
			if (this.selectedParticle != -1)
			{
				let mousePos = this.getMousePos(this.canvas, event);		
				this.sim.particles[this.selectedParticle].x = (mousePos.x - this.origin.x) / this.zoom;
				this.sim.particles[this.selectedParticle].y = -(mousePos.y - this.origin.y) / this.zoom;
			}
		}
		
		mouseUp(event) 
		{
			this.selectedParticle = -1;
		}
		
		wheel(event)
		{
			event.preventDefault();
			this.zoom += event.deltaY * -0.05;
			if (this.zoom < 1) 
				this.zoom = 1;
		}
	}
	
	gui = new GUI();
	gui.restart();