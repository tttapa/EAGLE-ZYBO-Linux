String img_path = "../../python/drone-images/image%04d.bmp";
String mask_path = "../../python/drone-images/mask%04d.bmp";

PImage img;
PImage mask;
int image_number = 1;
boolean show_mask = false;

void display() {
  image(show_mask ? mask : img, 0, 0);
}

boolean update() {
  img = loadImage(String.format(img_path, image_number));
  mask = loadImage(String.format(mask_path, image_number));
  if (img == null || mask == null)
    return false;
  display();
  println(image_number);
  return true;
}

void setup() {
  size(640, 480);
  update();
}

void draw() {
}

void keyPressed() {
  if (key == CODED) {
    if (keyCode == RIGHT) {
      image_number++;
      if (!update()) {
        image_number = 0;
        update();
      }
    } else if (keyCode == LEFT) {
      if (image_number > 0) {
        image_number--;
        update();
      }
    }
  }
}

void mousePressed() {
  if (mouseButton == LEFT) {
    update();
    textSize(20);
    fill(255, 30, 30);
    color c = img.get(mouseX, mouseY);
    c = color(blue(c), green(c), red(c)); // BGR → RGB
    text(String.format("%d°, %d, %d", 
      int(hue(c)), 
      int(saturation(c)), 
      int(brightness(c))), 
      width - 160, height - 20);
  } else if (mouseButton == RIGHT) {
    show_mask = !show_mask;
    display();
  }
}
