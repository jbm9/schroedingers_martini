dBig = 50;
dSmall = 30;
hCup = 53;

slope = (dBig - dSmall)/hCup/2;
echo(slope);


holeD = 4.5; // hole for mounting screw
screwDepth = 2.5; // depth of screw
postD = 7; // diameter of post
postH = 3.5;  // depth of post

minT = 2; // minimum thickness of side

slotW = 5; // width of slot for screwdriver

cBig = 47/2;
h = 15;
cSmall = cBig-h*slope;

ringW = 7;

echo(cBig);
echo(cSmall);

difference() {
  cylinder(h, cBig+ringW, cSmall+ringW, $fn=100);
  cylinder(h, cBig, cSmall, $fn=100);


  // screwdiver/grip slot
  translate([-slotW/2,-cBig-ringW-2, 0]) cube([slotW,ringW+10,h]);

  // mounting post hole
#  translate([0, cBig+ringW, minT+postD/2]) rotate([90, 0,0]) 
  union() { 
    cylinder(r=postD/2, h=postH, $fn=20);
    cylinder(r=holeD/2, h=ringW+2, $fn=20);
    translate([0,0,postH+screwDepth]) cylinder(r=postD/2, h=20, $fn=20);
  }
}
