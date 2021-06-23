// relay_enclosure.scad
// Copyright (c) 2021 Warren Taylor

use </home/wtaylor/data/openscad/MCAD/boxes.scad>

cli_fn = 32; // This value can be changed by the command line.
$fn = cli_fn;
fn_Low  = 32;
fn_Med  = fn_Low * 2;
fn_High = fn_Low * 4;

//$fn = $preview ? 12 : 72;

inchesToMm = 25.4;
wallWidth = 2.0;
supportWidth = 3.25;
spacing = 1 + (4.2 - 1.55);
gap = 0.5;
small_gap = 0.12;
omega = 0.001;


relay_board_width = 1.72;
controller_board_width = 4.0;
full_relay_board_width = 1.55;
full_controller_board_width = 4.0;

relay_board =     [138.1+gap*2, 50.26+gap*2, 1.72+gap*1];
controller_board = [88.0+gap*2, 56.40+gap*2, 1.55+gap*1];
board_separation = 47.0;

lower_enclosure_inside = [
    max(relay_board.x, controller_board.x) + gap*0,
    board_separation + relay_board.z + controller_board.z + spacing*3,
    max(relay_board.y, controller_board.y),
];
lower_enclosure_outside = [
    lower_enclosure_inside.x + wallWidth*2,
    lower_enclosure_inside.y + wallWidth*2,
    lower_enclosure_inside.z + wallWidth*1,
];

support_rail = [
    supportWidth,
    spacing * 2 + supportWidth,
    min(relay_board.y, controller_board.y),
];

upper_enclosure_inside = [
    0,
    0,
    0
];



lower_enclosure();
//circuit_boards();


module circuit_boards() {
    translate([0, (relay_board.z + board_separation)/2, relay_board.y/2])
    rotate([90,0,0])
    {
        cube(relay_board, center = true);
    }

    translate([0, -(controller_board.z + board_separation)/2, controller_board.y/2])
    rotate([90,0,0])
    {
        cube(controller_board, center = true);
    }
}


module lower_enclosure() {
    difference() {
        union() {
            // Outside & Inside walls of lower enclosure.
            translate([0, 0, lower_enclosure_inside.z/2]) {
                difference() {
                    translate([0, 0, -(wallWidth/2 + omega)]) {
                        cube(lower_enclosure_outside, center = true);
                    }
                    cube(lower_enclosure_inside, center = true);
                }//difference
            }//translate

            // Support Rails for relay board.
            for (offset = [-1, +1]) {
                translate([
                    offset * (lower_enclosure_inside.x/2 - support_rail.x/2 + omega),
                    lower_enclosure_inside.y/2 - support_rail.y/2 + omega,
                    support_rail.z/2 - omega
                ])
                {
                    cube(support_rail, center=true);
                }//translate
            }//for

            // Support Rails for controller board.
            for (offset = [-1, +1]) {
                translate([
                    offset * (controller_board.x*0.5 - support_rail.x*0 + omega),
                    -(lower_enclosure_inside.y/2 - support_rail.y/2 + omega),
                    support_rail.z/2 - omega
                ])
                {
                    cube([support_rail.x*2, support_rail.y, support_rail.z], center=true);
                }//translate
            }//for
        }//union

        //++ difference ++
        #circuit_boards();
    }
}


module upper_enclosure() {

}
