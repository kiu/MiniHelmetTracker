$fn = 60;

base_thick = 2.0;
holder_width = 26.0;
recept_width = holder_width + 1 + 2;

height = 18;

//--------------------------------------------------------

grab();
translate ([1.5, 18, 2]) holder();
translate ([recept_width/2 - 4/2, 0, 20]) rotate([0, 90, 0]) horn();

//--------------------------------------------------------
module grab() {
	  rotate([0, 90, 0]) translate([-8, 0, 0]) servo_mount_big();	  
	  translate([0, 8, 8]) rotate([90, 0, 0]) servo_mount_small();
	  translate([0, -8, 0]) cube([7, 8, 8]);
	  translate([0, 8, 0]) receptacle();
}

//--------------------------------------------------------

module servo_mount_small() {
	  difference() {
			cube([7, 7, 16]);
			translate([2, 2, 2]) cube([3, 5, 12]);
			translate([7/2, 5/2 + 2, 16 / 2]) rotate([0, 90, 0]) cylinder(h=7, d=2.2, center = true);
	  }
}

//--------------------------------------------------------

module servo_mount_big() {
	  servo_height = recept_width;
	  servo_gear = 5.9;
	  servo_gear_height = 3.0;
	  servo_screw = 3.2;
	  servo_screw_channel = 2.0;
	  servo_screw_head = 6.4;
	  servo_block = 8;
	  
	  difference() {
			cube([servo_block, servo_block, servo_height]);
			translate([servo_block / 2, servo_block / 2, 0]) {
				  translate([0, 0, servo_height - servo_gear_height]) cylinder(h=servo_gear_height, d=servo_gear);
				  translate([0, 0, servo_height - servo_gear_height - servo_screw_channel]) cylinder(h=servo_screw_channel, d=servo_screw);
				  translate([0, 0, 0]) cylinder(h=servo_height - servo_gear_height - servo_screw_channel, d=servo_screw_head);
			}
	  }
}

//--------------------------------------------------------

module receptacle() {
	  recept_length = 36.0;
	  recept_capture = 4.0;
	  recept_empty = 6;
	  
	  cube([recept_width, recept_length, base_thick]);
	  
	  half2 = (base_thick + recept_capture) / 3 * 2;
	  half1 = (base_thick + recept_capture) / 3 * 1;
	  
	  translate([0, 0, 0]) cube([1, recept_length, half2]);
	  translate([0.06, 0, half2 - 0.35]) rotate([0, -20, 0]) cube([1, recept_length, half1]);
	  	  
	  translate([recept_width - 1, 0, 0]) cube([1, recept_length, half2]);
	  translate([recept_width - 1, 0, half2]) rotate([0, 20, 0]) cube([1, recept_length, half1]);
	  
	  h = base_thick + recept_capture + recept_empty + recept_capture;
	  translate([14.5, 27, 2]) cylinder(10,9,0,$fn=4);
}

//--------------------------------------------------------

module holder() {
difference() {
union() {
	intersection() {
	  cube([holder_width, 32+20, base_thick]);
	  translate([0, 32, 0]) rotate([0,0,30]) {
			cylinder(h=base_thick, d=holder_width*2);
			cube([holder_width, 48, base_thick]);
	  }
	}
	
	cube([holder_width, 32, base_thick]);
	translate([0, 32, 0]) rotate([0,0,30]) {
		cube([holder_width, 48, base_thick]);
		translate([0, 48-27, 0]) {
			difference() {
				  cube([holder_width, 8, 20]);
				  translate([0, 0, 20 - 2 - 2]) cube([2, holder_width, 2]);
				  translate([holder_width-2, 0, 20 - 2 - 2]) cube([2, holder_width, 2]);
			}
		}
    }
	}

translate([13, 25-8, 0]) cylinder(10,9,0,$fn=4);	
	}
}

//--------------------------------------------------------

module horn() {
	  servo_height = 4;
	  servo_gear = 4.9;
	  servo_gear_height = 3.0;
	  servo_screw = 2.2;
	  servo_screw_channel = 1.0;
	  servo_screw_head = 5;
	  servo_block = 7;
	  servo_arm = 26;
	  
	  difference() {
			union() {
				  cylinder(h=servo_height, d=servo_block);
				  translate([-servo_block / 2, 0, 0]) cube([servo_block, servo_arm, servo_height]);
				  translate([0, servo_arm, 0]) cylinder(h=servo_height, d=servo_block);
			}
			translate([0, 0, servo_height - servo_gear_height]) cylinder(h=servo_gear_height, d=servo_gear);
			translate([0, 0, servo_height - servo_gear_height - servo_screw_channel]) cylinder(h=servo_screw_channel, d=servo_screw);
			translate([0, 0, 0]) cylinder(h=servo_height - servo_gear_height - servo_screw_channel, d=servo_screw_head);
	  }
}

//--------------------------------------------------------