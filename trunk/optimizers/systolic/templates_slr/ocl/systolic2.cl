
#include "systolic.h"

__kernel
__attribute__ ((reqd_work_group_size(1, 1, 1)))
//__attribute__ ((xcl_dataflow))
void systolic_top2(__global data_bitwidth *ifmap, __global data_bitwidth *weight, __global data_bitwidth *ofmap)
{
	for (int o2 = 0; o2 < O; o2++)
	for (int o1 = 0; o1 < BS_O; o1++)
	for (int r1 = 0; r1 < BS_R; r1++)
	{
		int ofmap_idx = (o2 * BS_O + o1) * BS_R + r1;

		ofmap[ofmap_idx] = 0;
		for (int i1 = 0; i1 < BS_I; i1++)
		{
			int ifmap_idx = r1 * BS_I + i1;
			int wt_idx = (o2 * BS_O + o1) * BS_I + i1;

			ofmap[ofmap_idx] += weight[wt_idx] * ifmap[ifmap_idx];
		}
	}
}
