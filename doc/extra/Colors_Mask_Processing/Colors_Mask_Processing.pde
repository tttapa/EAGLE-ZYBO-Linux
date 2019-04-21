final PVector red_xy = new PVector(0.0, 1.0);
final PVector green_xy = new PVector(sin(2.0*PI/3.0), cos(2.0*PI/3.0));
final PVector blue_xy = new PVector(sin(-2.0*PI/3.0), cos(-2.0*PI/3.0));

void setup() {
  size(1786, 766);
  PGraphics pg = createGraphics(width, height);
  pg.beginDraw();
  pg.background(0);
  for (int x = 0; x < width/2; ++x) {
    for (int y = 0; y < height; ++y) {
      PVector current = new PVector(x-width/4, 255-y);
      float r = max(current.dot(red_xy), 0);
      float g = max(current.dot(green_xy), 0);
      float b = max(current.dot(blue_xy), 0);
      r = round(r);
      g = round(g);
      b = round(b);
      boolean valid = max(max(r, g), b) <= 255;
      if (valid) {
        pg.stroke(color(r, g, b));
        pg.point(x, y);
        PVector current_color = new PVector(r, g, b);
        pg.stroke(f(current_color));
        pg.point(width/2+x, y);
      }
    }
  }
  pg.stroke(color(255, 255, 255));
  pg.point(442, 255);
  pg.point(width/2+442, 255);
  pg.endDraw();
  pg.save("../../images/Colors-Mask.png");
  image(pg, 0, 0);
}

color f(PVector col) {
  final float thres = 6;
  float v = (col.x / 4 - (col.y + col.z)/2)/2;
  int c = max(0, min(int(round(8 * v)), 255));
  return !(v >= thres) ? color(col.x, col.y, col.z) : color(c);
}

void draw() {}