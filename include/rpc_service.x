/*
 * rpc.x - Specification of remote procedure calls to the server.
 */

/*
 * Define 2 procedurs :
 * date_1() accepts a long and returns a string. 
 * cpu_1() accepts a long and returns a float. 
 */

program RPC_PROG {
    version INIT_VERS {
	string	DATE(long) = 1;      /* procedure number = 1 */ 
    float	CPU(long) = 2;       /* procedure number = 2 */ 
    float	MEMORY(long) = 3;    /* procedure number = 3 */ 
	string	PROCESSES(long) = 4; /* procedure number = 4 */ 
    } = 1;			             /* version number = 1 */
} = 0x31234567;			         /* program number */

