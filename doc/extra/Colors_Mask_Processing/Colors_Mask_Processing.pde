final PVector red_xy = new PVector(0.0, 1.0);
final PVector green_xy = new PVector(sin(2.0*PI/3.0), cos(2.0*PI/3.0));
final PVector blue_xy = new PVector(sin(-2.0*PI/3.0), cos(-2.0*PI/3.0));

enum LightDark {
  light, 
    dark,
};

PGraphics createTriangles(LightDark lightdark) {
  PGraphics pg = createGraphics(width, height);
  pg.beginDraw();
  pg.background(0);
  for (int x = 0; x < width/2; ++x) {
    for (int y = 0; y < height; ++y) {
      PVector current;
      if (lightdark == LightDark.dark)
        current = new PVector(x-width/4, 255-y);
      else
        current = new PVector(width/4-x, y-height+255);
      float r = max(current.dot(red_xy), 0);
      float g = max(current.dot(green_xy), 0);
      float b = max(current.dot(blue_xy), 0);
      r = round(r);
      g = round(g);
      b = round(b);
      boolean valid = max(max(r, g), b) <= 255;
      if (valid) {
        if (lightdark == LightDark.light) {
          r = 255-r;
          g = 255-g; 
          b = 255-b;
        }
        pg.stroke(color(r, g, b));
        pg.point(x, y);
        PVector current_color = new PVector(r, g, b);
        pg.stroke(f(current_color));
        pg.point(width/2+x, y);
      }
    }
  }
  pg.stroke(color(255, 255, 255));
  pg.point(width/4, 255);
  pg.point(3*width/4, 255);
  pg.endDraw();
  return pg;
}

PGraphics light;
PGraphics dark;
LightDark displayLightDark = LightDark.light;

void setup() {
  size(1786, 766);
  light = createTriangles(LightDark.light);
  light.save("../../images/Colors-Mask-Light.png");
  dark = createTriangles(LightDark.dark);
  dark.save("../../images/Colors-Mask-Dark.png");
  image(light, 0, 0);
}

color f(PVector col) {
  final float thres = 6;
  float v = (col.x / 4 - (col.y + col.z)/2)/2;
  int c = max(0, min(int(round(8 * v)), 255));
  return !(v >= thres) ? color(col.x, col.y, col.z) : color(c);
}

void draw() {
}

void mouseClicked() {
  if (displayLightDark == LightDark.light) {
    displayLightDark = LightDark.dark;
    image(dark, 0, 0);
  } else {
    displayLightDark = LightDark.light;
    image(light, 0, 0);
  }
}
