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
        pg.stroke(f1(r, g, b));
        pg.point(width/2+(x/2), (y/2));
        pg.stroke(f2(r, g, b));
        pg.point(3*width/4+(x/2), (y/2));
        pg.stroke(f3(r, g, b));
        pg.point(width/2+(x/2), (y/2)+height/2);
        pg.stroke(f4(r, g, b));
        pg.point(3*width/4+(x/2), (y/2)+height/2);
      }
    }
  }
  pg.stroke(color(255, 255, 255));
  pg.point(width/4, 255);
  pg.point(3*width/4, 255);
  pg.endDraw();
  return pg;
}

PGraphics createGradient(float hue) {
  PGraphics pg = createGraphics(width, height);
  pg.beginDraw();
  pg.background(0);
  for (int x = 0; x < width/2; ++x) {
    for (int y = 0; y < height; ++y) {
      colorMode(HSB, 255);
      color c = color(hue, 2 * 255.0 * x / width, 255.0 * (height - y) / height);
      colorMode(RGB, 255);
      float r = red(c);
      float g = green(c);
      float b = blue(c);
      pg.stroke(color(r, g, b));
      pg.point(x, y);
      pg.stroke(f1(r, g, b));
      pg.point(width/2+(x/2), (y/2));
      pg.stroke(f2(r, g, b));
      pg.point(3*width/4+(x/2), (y/2));
      pg.stroke(f3(r, g, b));
      pg.point(width/2+(x/2), (y/2)+height/2);
      pg.stroke(f4(r, g, b));
      pg.point(3*width/4+(x/2), (y/2)+height/2);
    }
  }
  pg.endDraw();
  return pg;
}

PGraphics light;
PGraphics dark;
PGraphics tests;

void setup() {
  size(1786, 766);      
  colorMode(RGB, 255);
  light = createTriangles(LightDark.light);
  light.save("../../images/Colors-Mask-Light.png");
  dark = createTriangles(LightDark.dark);
  dark.save("../../images/Colors-Mask-Dark.png");
  tests = createGradient(0);
  tests.save("../../image/Colors-Mask-Gradient.png");
  update();
}

// /*
color f1(float r, float g, float b) {
  float v = (r/8 - (g+b)/4) >= 6 ? 1 : -1;
  int rr = max(0, min(int(round(255 * v)), 255));
  int bb = max(0, min(int(round(-255 * v)), 255));
  return color(rr, 0, bb);
} // */

// Calculates the base-2 logarithm of a number
float log2(float x) {
  return (log(x) / log(2));
}

final int SATURATION_THRES       = 16;
final int SATURATION_THRES_SHIFT = round(log2(SATURATION_THRES));
final int BRIGHTNESS_THRES       = 16;
final int HUE_THRES_DEGREES      = 15;
final int HUE_THRES_SHIFT        = round(log2(60 / HUE_THRES_DEGREES));

// /*
color f2(float r, float g, float b) {
  int max = int(round(max(max(r, g), b)));
  int min = int(round(min(min(r, g), b)));
  int delta = max - min;
  boolean cond = delta > max >> SATURATION_THRES_SHIFT // saturation
    && max > BRIGHTNESS_THRES // brightness
    && r == max && delta >> HUE_THRES_SHIFT > abs(g - b); // hue
  float v = cond ? 1 : -1;
  int rr = max(0, min(int(round(255 * v)), 255));
  int bb = max(0, min(int(round(-255 * v)), 255));
  return color(rr, 0, bb);
} // */

// /*
color f3(float r, float g, float b) {
  // float v = r * r + r > (b + g) * (g + b - r) + 255 * 8 && r * 255 > (g + b) * (g + b) && (g+b) * 128  <= (2 * r - b - g) * (2 * r - b - g)  ? 1: -1;
  // float v = g * g + b * b + 255*8 < r * r - 8 * r ? 1: -1;
  float v = g * g + b * b + 255*8 < r * r - 8 * r && ((r/8 - (g+b)/4) >= 0 || (g + b)*(g+b) - (g+b)*32 < r * 128) ? 1: -1;
  int rr = max(0, min(int(round(255 * v)), 255));
  int bb = max(0, min(int(round(-255 * v)), 255));
  return color(rr, 0, bb);
} // */

// /*
color f4(float r, float g, float b) {
  float h = hue(color(r, g, b));
  float s = saturation(color(r, g, b));
  float v = brightness(color(r, g, b));
  float res = (h > 240 || h < 16) && s > 16 && v > 16 ? 1: -1;
  int rr = max(0, min(int(round(255 * res)), 255));
  int bb = max(0, min(int(round(-255 * res)), 255));
  return color(rr, 0, bb);
} // */

void draw() {
}

int display = 2;

void mouseClicked() {
  update();
}
void update() {
  switch(display) {
  case 0:
    image(light, 0, 0);
    break;
  case 1:
    image(dark, 0, 0);
    break;
  case 2:
    image(tests, 0, 0);
    break;
  }
  display++;
  if (display == 3)
    display = 0;
}
