	#include <coda-c.h>
	#include <math.h>

	struct Shape_ { Char color; };
#define class Shape
CodaClassZerosC(); // # 1. ABSTRACTION: Define an abstract blueprint for all shapes
CodaClass(Shape,struct Shape_,Root);
	propertyO$(Char,color); // # 2. ENCAPSULATION: Private attribute

	typedef double OSig$(calculate_area);      // Abstract method; must be implemented by subclasses.
	typedef double OSig$(calculate_perimeter); // Abstract method; must be implemented by subclasses.
#undef class // Shape

	struct Circle_ { double radius; };
#define class Circle
CodaClassZerosC(); // # 3. INHERITANCE: Circle inherits from Shape
CodaClass(Circle,struct Circle_,Shape);
	property$$(double,radius);

class Circle_new(Char color,double radius) {
	class self=newO(class);
	Shape_set_color((Obj)self,color);
	_ radius=radius;
	return(self);
	}

method$(double,calculate_area) { // # 4. POLYMORPHISM: Provide specialized logic for Circle
	return(M_PI * _ radius * _ radius);
	}

method$(double,calculate_perimeter) {
	return(2.0 * M_PI * _ radius);
	}
#undef class // Circle

	struct Rectangle_ { double width,height; };
#define class Rectangle
CodaClassZerosC();
CodaClass(Rectangle,struct Rectangle_,Shape);
	property$$(double,width);
	property$$(double,height);

class Rectangle_new(Char color,double width,double height) {
	class self=newO(class);
	Shape_set_color((Obj)self,color);
	_ width=width;
	_ height=height;
	return(self);
	}

method$(double,calculate_area) { return _ width * _ height; }

method$(double,calculate_perimeter) { return 2.0 * (_ width + _ height); }

#undef class // Rectangle

// # --- 3. POLYMORPHISM IN ACTION (Usage Example) ---
void print_shape_details(Obj shape) {
    // This function accepts any object that is a subclass of Shape.
    printf("--- %12.12s  Details  Color:%-8.8s  ",kindO(shape),Shape_color(shape));
    printf("Area: %3.2f  ",obj_(calculate_area,shape));
    printf("Perimeter: %3.2f  ---\n",obj_(calculate_perimeter,shape));
	}

int main() {
	printf("M_PI %3.7f\n",M_PI);
    Circle my_circle = Circle_new(Os("Blue"),5.0);
    Obj my_rectangle = Rectangle_new(Os("Green"),4.0,6.0);

    // # The same function handles different shape types uniformly
    print_shape_details(my_circle);
    print_shape_details(my_rectangle);
	}

// OUTPUT:
// M_PI 3.1415927
// ---       Circle  Details  Color:Blue      Area: 78.54  Perimeter: 31.42  ---
// ---    Rectangle  Details  Color:Green     Area: 24.00  Perimeter: 20.00  ---
