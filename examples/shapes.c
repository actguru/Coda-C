
	#include <coda-c.h> // shapes.c  08/20/2026
	#include <math.h>   // Coda-C subclass examples

	struct Shape_ { Char color; };
#define class Shape
CodaClassZerosC(); // 1. ABSTRACTION: blueprint for all shapes
CodaClass(Shape,struct Shape_,Root);
	propertyO$(Char,color); // 2. ENCAPSULATION: Private data
	typedef double OSig$(calculate_area); // Abstract method: required in subclasses
	typedef double OSig$(calculate_perimeter); // defines function type and signature key
#undef class // Shape

	struct Circle_ { double radius; };
#define class Circle
CodaClassZerosC(); // 3. INHERITANCE: Circle inherits from Shape
CodaClass(Circle,struct Circle_,Shape);
	getter$(double,radius,_ radius); // double Circle_radius(Circle self) { return _ radius; }
	method$(double,calculate_area) { return(M_PI * _ radius * _ radius); }
	method$(double,calculate_perimeter) { return(2.0 * M_PI * _ radius); }

class Circle_new(Char color,double radius) {
	class self=newO(class);
	Shape_set_color((Obj)self,color);
	_ radius=radius; // where #define _ self->
	return(self);
	}
#undef class // Circle

	struct Rectangle_ { double width,height; };
#define class Rectangle
CodaClassZerosC();
CodaClass(Rectangle,struct Rectangle_,Shape);
	method$(double,calculate_area) { return _ width * _ height; }
	method$(double,calculate_perimeter) { return 2.0 * (_ width + _ height); }

class Rectangle_new(Char color,double width,double height) {
	class self=newO(class);
	Shape_set_color((Obj)self,color);
	_ width=width; _ height=height;
	return(self);
	}
#undef class // Rectangle


// 4. POLYMORPHISM (Usage Example)
void print_shape_details(Obj shape) { // This function accepts any subclass of Shape.
    printf("--- %12.12s  Details  Color:%-8.8s  ",kindO(shape),Shape_color(shape));
    printf("Area: %3.2f  ",obj_(calculate_area,shape));
    printf("Perimeter: %3.2f  ---\n",obj_(calculate_perimeter,shape));
	}

 int main() {
    Circle my_circle = Circle_new(Os("Blue"),5.0);
    Obj my_rectangle = Rectangle_new(Os("Green"),4.0,6.0);

    print_shape_details(my_circle);
    print_shape_details(my_rectangle);
	}
// OUTPUT:
// ---       Circle  Details  Color:Blue      Area: 78.54  Perimeter: 31.42  ---
// ---    Rectangle  Details  Color:Green     Area: 24.00  Perimeter: 20.00  ---
