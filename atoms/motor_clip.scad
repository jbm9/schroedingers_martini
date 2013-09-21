/*
 * motor_clip.scad: the clip for the motors
 */

scale=25.4; // measurements are in inches...

width=30 + 0.5; // width of motor body
height=12 + 0.5; // "height" of motor on its side
depth=11;         // how deep to make the bracket

mountD = 2.5; // diameter of mounting screw
mountDX = 3; // center dx from edge
mountDY = 6;

flange=10; // 10mm flange
hole = 4; // diameter of hole
walld = 4; // how thick to make the walls

union() {

  // left flange
    translate([0,
	       height,
	       0]) 
  difference() {
      cube([flange+2*mountDX, walld, depth]);
      translate([flange/2,
		 -0.5,
		 depth/2])
	rotate([-90,0,0]) # cylinder(r=walld/2, h=walld*2, $fn=10);
    }

  // clip body
#  translate([flange,
	     0,
	     0])
    difference() {
    cube([2*mountDX+width+2*mountDX,
	  walld+height,
	  depth]);
    translate([2*mountDX,walld,0]) cube([width,height+1,depth]);

    // screwdowns for the motor's brackets
    translate([mountDX,
	       walld+mountDY,
	       0])
      cylinder(r=mountD/2, h=depth, $fn=10);

    translate([width+3*mountDX,
	       walld+mountDY,
	       0])
      cylinder(r=mountD/2, h=depth, $fn=10);

    


  }

  // right flange
  translate([2*mountDX+flange+width,
	     height,
	     0]) 
  difference() {
    cube([flange+walld, walld, depth]);
      translate([walld+flange/2,
		 -0.5,
		 depth/2])
	rotate([-90,0,0]) # cylinder(r=walld/2, h=walld*2, $fn=10);
  }


}


