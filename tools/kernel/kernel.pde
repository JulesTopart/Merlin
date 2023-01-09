
final float M_PI = 3.141592653589793;
final float H = 8;           // kernel radius
final float H2 = H * H;        // radius^2 for optimization

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

void setup(){
  size(1080,720, P3D);
  
  
 
}

PVector origin;
void draw(){
  
  background(0);
  translate(width/2, height/2);
  stroke(255);
  
  int grid = 100;
  float gridSpace = 300.0f / float(grid);
  
  scale(1.5);
  translate(0, 0, 300);
  
  translate(-150, 0, -150.0f);
  rotateX(-3.1415926/6.0);
  translate(0, 0, 100);
  rotateY(1.5);

  
  origin = new PVector((-mouseY+height)/600.0f*(grid/2.0f),mouseX/600.0f*(grid/2.0f),0);
  println(origin);
  
  for(float x = 0; x < grid; x+=0.8){
    for(float y = 0; y < grid; y+=0.8){
      PVector particle = new PVector(x,y,0);
      float p6 = poly6Kernel(origin.copy().sub(particle));
      float sp = spikyKernel(origin.copy().sub(particle));
      if(mousePressed) {
        stroke(p6*300000.0, 80, 80);
        point(x*gridSpace, -p6*3000.0f, y*gridSpace);
      }else{
        stroke(sp*300000.0, 80, 80);
        point(x*gridSpace, -sp*3000.0f, y*gridSpace);
      }
    }
  }
  
}
