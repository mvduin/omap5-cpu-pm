#include "defs.h"
#include "map-phys.h"
#include "die.h"
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

// generic power domain management
struct Power {
	// some or all ctrl fields may be readonly
	// individual power domains may support only a subset of states.
	// the actual number of memory banks (and their supported states) will vary.

	unsigned ctrl;
	// bits  0- 1	rw  requested domain state:  0 off, 1 ret, 2 idle, 3 on
	// bit   2	rw  dff ret-state:  0 off (oswr), 1 ret (cswr)
	// bit   3	z-
	// bit   4	rx  request low power state change (auto-clears)
	// bits  5- 6	z-
	// bit   7	rw  forced off (cortex-a15 core 1 only)
	// bit   8	rw  memory bank 0 ret-state:  0 off, 1 ret
	// ...		...
	// bit  15	rw  memory bank 7 ret-state:  0 off, 1 ret
	// bits 16-17	rw  memory bank 0 on-state:  0 off, 1 ret, 3 on
	// ...		...
	// bits 30-31	rw  memory bank 7 on-state:  0 off, 1 ret, 3 on

	unsigned stat;
	// bits  0- 1	r-  current domain state:  0 off, 1 ret, 2 idle, 3 on
	// bit   2	r-  logic state:  0 off, 1 on
	// bit   3	z-
	// bits  4- 5	r-  memory bank 0 state:  0 off, 1 ret, 3 on
	// ...		...
	// bits 18-19	r-  memory bank 7 state:  0 off, 1 ret, 3 on
	// bit  20	r-  transition in progress
	// bits 21-23	z-
	// bits 24-25	rs  last domain lp state:  0 off, 1 ret, 2 idle, 3 none
	// bits 26-31	z-
};

// for the cortex-a15 core power management:
//	the only writable register bits are:
//		ctrl bits 0-1
//		ctrl bit 7 (for core 1 only)
//		stat bits 24-25
//	there's one memory bank, which is the core's L1 cache
//	all logic and memory is retained in retention mode
//	only core 1 supports off-mode
let static &core0_pm = map_phys<Power>( 0x482'43'400 );
let static &core1_pm = map_phys<Power>( 0x482'43'800 );

static constexpr char const *state_names[4] = { "OFF", "RETENTION", "INACTIVE", "ON" };

let static do_core_pm( Power &pm, unsigned new_state )
{
	assert( new_state < 4 );
	let ctrl = pm.ctrl;
	let stat = pm.stat;
	if( new_state == ( ctrl & 3 ) )
		new_state = 0;
	printf( "\tcurrent power state: %s\n", state_names[ stat & 3 ] );
	printf( "\trequested power state: %s\n", state_names[ ctrl & 3 ] );
	if( ( stat & 3 ) == 3 && ( stat >> 24 & 3 ) < 3 ) {
		printf( "\tlowest power state reached: %s\n", state_names[ stat >> 24 & 3 ] );
		pm.stat = stat | 3 << 24;
	}
	if( stat >> 20 & 1 )
		printf( "\tpower state is transitioning\n" );
	if( ctrl >> 7 & 1 ) {
		printf( "\tcore is forced OFF\n" );
		if( new_state )
			printf( "\trefusing to change requested power state\n" );
		return;
	}
	if( ! new_state )
		return;
	if( !( ctrl & 3 ) ) {
		printf( "\trefusing to change requested power state\n" );
		return;
	}
	printf( "\tchanging requested power state to: %s\n", state_names[ new_state ] );
	printf( "\t---\n" );
	pm.ctrl = ( ctrl & ~3 ) | new_state;
	asm( "" ::: "memory" );
	do_core_pm( pm, 0 );
}

let main( int argc, char *argv[] ) -> int
{
	let const argend = argv + argc;
	let const arg0 = *argv++;
	let state = 0u;
	if( argv < argend ) {
		let arg = *argv++;
		if( strcmp( arg, "--retention" ) == 0 )
			state = 1;
		else if( strcmp( arg, "--inactive" ) == 0 )
			state = 2;
		else if( strcmp( arg, "--on" ) == 0 )
			state = 3;
		else
			--argv;
	}
	if( argv < argend )
		die( "Usage: %s [--on|--inactive|--retention]\n", arg0 );
	printf( "core 0 power management:\n" );
	do_core_pm( core0_pm, state );
	printf( "core 1 power management:\n" );
	do_core_pm( core1_pm, state );
	return 0;
}
