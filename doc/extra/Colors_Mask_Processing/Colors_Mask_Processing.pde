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
        pg.stroke(f(r, g, b));
        pg.point(width/2+(x), (y));
      }
    }
  }
  pg.ellipseMode(CENTER);
  if (lightdark == LightDark.dark) {
    pg.stroke(255);
    pg.fill(255);
    pg.circle(width/4, 255, 2);
    pg.circle(3*width/4, 255, 2);
  } else {
    pg.stroke(0);
    pg.fill(0);
    pg.circle(width/4, height-255, 2);
    pg.circle(3*width/4, height-255, 2);
  }
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
      pg.stroke(f(r, g, b));
      pg.point(width/2+(x), (y));
    }
  }
  pg.endDraw();
  return pg;
}

PGraphics light;
PGraphics dark;
PGraphics gradient;

void setup() {
  size(1786, 766);      
  colorMode(RGB, 255);
  light = createTriangles(LightDark.light);
  light.save("../../images/Colors-Mask-Light.png");
  dark = createTriangles(LightDark.dark);
  dark.save("../../images/Colors-Mask-Dark.png");
  gradient = createGradient(0);
  gradient.save("../../images/Colors-Mask-Gradient.png");
  update();
}

// Calculates the base-2 logarithm of a number
float log2(float x) {
  return (log(x) / log(2));
}

/*
final int SATURATION_THRES       = 32;
final int SATURATION_THRES_SHIFT = 8 - round(log2(SATURATION_THRES));
final int BRIGHTNESS_THRES       = 32;
final int HUE_THRES_DEGREES      = 30;
final int HUE_THRES_SHIFT        = round(log2(60 / HUE_THRES_DEGREES));

color f(float r, float g, float b) {
  int max = int(round(max(max(r, g), b)));
  int min = int(round(min(min(r, g), b)));
  int delta = max - min;
  boolean cond = delta > max >> SATURATION_THRES_SHIFT // saturation
    && max > BRIGHTNESS_THRES // brightness
    && r == max && delta >> HUE_THRES_SHIFT > abs(g - b) // hue
    && ((max / 16) * (max / 32) + delta) / 2 > max / 4;
  return cond ? color(255 - HUE_THRES_DEGREES * abs(g - b) / delta * 4) : color(r, g, b);
} // */


// /*
final int SATURATION_THRES       = 32;
final int SATURATION_THRES_SHIFT = 8 - round(log2(SATURATION_THRES));
final int BRIGHTNESS_THRES       = 30;
final int HUE_THRES_DEGREES      = 30;
final int HUE_THRES_SHIFT        = round(log2(60 / HUE_THRES_DEGREES));

color f(float r, float g, float b) {
  int max = int(round(max(max(r, g), b)));
  int min = int(round(min(min(r, g), b)));
  int delta = max - min;
  boolean cond = delta > max >> SATURATION_THRES_SHIFT // saturation
    && max > BRIGHTNESS_THRES // brightness
    && r == max && delta >> HUE_THRES_SHIFT > abs(g - b); // hue
  return cond ? color(255 - HUE_THRES_DEGREES * abs(g - b) / delta * 4) : color(r, g, b);
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
    image(gradient, 0, 0);
    break;
  }
  display++;
  if (display == 3)
    display = 0;
}
