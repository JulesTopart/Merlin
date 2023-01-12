
final float M_PI = 3.141592653589793;
final float H = 8;           // kernel radius
final float H2 = H * H;        // radius^2 for optimization

float rotX = 0.0f;

float poly6Kernel(PVector r) {
    if(r.mag()*r.mag() > H2) return 0;

    float r2 = PVector.dot(r, r);
    float a = 315.0/(64.0* M_PI * pow(H, 9));
    return a * pow(H2-r2, 3);
}


float spikyKernel(PVector r) {
    if (r.mag()*r.mag() >= H2) {
        return 0.0;
    }
    return 15.0/(M_PI * pow(H, 6)) * pow(H-r.mag(), 3);
}

float cubicSplineKernel(PVector r) {
  float q = r.mag() / H;
    if (q >= 0 && q < 1) {
        return 1 - (3.0/2.0)*q*q + (3.0/4.0)*q*q*q;
    }else if (q >= 1 && q <= 2) {
        return (1.0/4.0)*pow((2-q),3);
    }else return 0.0f;
}

float hyperbolicSplineKernel(PVector r){
    float q = r.mag() / H;
    if (q >= 0 && q < 1) {
        return q*q*q - 6*q +6;
    }else if (q >= 1 && q <= 2) {
        return pow(2-q,3);
    }else return 0.0f;
}

void setup(){
  size(1080,720, P3D);
  
  
 
}

PVector origin;
void draw(){
  
  background(255);
  translate(width/2, height/2 + 100);
  stroke(255);
  
  int grid = 100;
  float gridSpace = 300.0f / float(grid);
  
  scale(1.5);
  translate(0, 0, 300);
  
  translate(-150, 0, -150.0f);
  rotateX(rotX - 3.1415926/6.0);
  translate(0, 0, 100);
  rotateY(1.5);

  
  origin = new PVector((-mouseY+height)/600.0f*(grid/2.0f),mouseX/600.0f*(grid/2.0f),0);
  
  for(float x = 0; x < grid; x+=0.8){
    for(float y = 0; y < grid; y+=0.8){
      PVector particle = new PVector(x,y,0);
      float p6 = poly6Kernel(origin.copy().sub(particle));
      float sp = spikyKernel(origin.copy().sub(particle));
      float cs = cubicSplineKernel(origin.copy().sub(particle));
      float hs = hyperbolicSplineKernel(origin.copy().sub(particle));
      if(key == 'a') {
        stroke(p6*300000.0, 10, 10);
        point(x*gridSpace, -p6*3000.0f, y*gridSpace);
      }else if(key == 'z'){
        stroke(hs*3.0,10, 10);
        point(x*gridSpace, -hs*3.0f, y*gridSpace);
      }else if(key == 'e'){
        stroke(cs*300.0,10, 10);
        point(x*gridSpace, -cs*30.0f, y*gridSpace);
      }else{
        stroke(sp*300000.0, 10, 10);
        point(x*gridSpace, -sp*3000.0f, y*gridSpace);
      }
    }
  }
  
}

void mouseDragged(){
  rotX += -0.08*(mouseY-pmouseY);
}
