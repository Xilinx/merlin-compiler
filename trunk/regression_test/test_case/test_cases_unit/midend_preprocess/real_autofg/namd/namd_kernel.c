//(C) Copyright 2016-2021 Xilinx, Inc.
//All Rights Reserved.
//
//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License. (edited)

#define NUM_CELLS	700
#define NA_BRAM_SIZE    27*512*4
#define DIM_X	10
#define DIM_Y	10
#define DIM_Z	7

int i_cell_cnt (int cell_index, int default_cell_size) 
{

    int i_cell_cnt_array[NUM_CELLS];

    if (default_cell_size != -1) return default_cell_size;

    i_cell_cnt_array[0   ] = 140;		
    i_cell_cnt_array[1   ] = 148;
    i_cell_cnt_array[2   ] = 156;
    i_cell_cnt_array[3   ] = 155;
    i_cell_cnt_array[4   ] = 150;
    i_cell_cnt_array[5   ] = 152;
    i_cell_cnt_array[6   ] = 82;
    i_cell_cnt_array[7   ] = 139;
    i_cell_cnt_array[8   ] = 167;
    i_cell_cnt_array[9   ] = 170;
    i_cell_cnt_array[10  ] = 157;
    i_cell_cnt_array[11  ] = 159;
    i_cell_cnt_array[12  ] = 168;
    i_cell_cnt_array[13  ] = 94;
    i_cell_cnt_array[14  ] = 143;
    i_cell_cnt_array[15  ] = 172;
    i_cell_cnt_array[16  ] = 169;
    i_cell_cnt_array[17  ] = 165;
    i_cell_cnt_array[18  ] = 167;
    i_cell_cnt_array[19  ] = 172;
    i_cell_cnt_array[20  ] = 97;
    i_cell_cnt_array[21  ] = 146;
    i_cell_cnt_array[22  ] = 148;
    i_cell_cnt_array[23  ] = 158;
    i_cell_cnt_array[24  ] = 158;
    i_cell_cnt_array[25  ] = 153;
    i_cell_cnt_array[26  ] = 166;
    i_cell_cnt_array[27  ] = 93;
    i_cell_cnt_array[28  ] = 151;
    i_cell_cnt_array[29  ] = 140;
    i_cell_cnt_array[30  ] = 161;
    i_cell_cnt_array[31  ] = 157;
    i_cell_cnt_array[32  ] = 157;
    i_cell_cnt_array[33  ] = 151;
    i_cell_cnt_array[34  ] = 84;
    i_cell_cnt_array[35  ] = 148;
    i_cell_cnt_array[36  ] = 142;
    i_cell_cnt_array[37  ] = 159;
    i_cell_cnt_array[38  ] = 157;
    i_cell_cnt_array[39  ] = 150;
    i_cell_cnt_array[40  ] = 161;
    i_cell_cnt_array[41  ] = 90;
    i_cell_cnt_array[42  ] = 146;
    i_cell_cnt_array[43  ] = 146;
    i_cell_cnt_array[44  ] = 166;
    i_cell_cnt_array[45  ] = 150;
    i_cell_cnt_array[46  ] = 142;
    i_cell_cnt_array[47  ] = 164;
    i_cell_cnt_array[48  ] = 94;
    i_cell_cnt_array[49  ] = 148;
    i_cell_cnt_array[50  ] = 167;
    i_cell_cnt_array[51  ] = 167;
    i_cell_cnt_array[52  ] = 167;
    i_cell_cnt_array[53  ] = 161;
    i_cell_cnt_array[54  ] = 173;
    i_cell_cnt_array[55  ] = 92;
    i_cell_cnt_array[56  ] = 146;
    i_cell_cnt_array[57  ] = 149;
    i_cell_cnt_array[58  ] = 171;
    i_cell_cnt_array[59  ] = 160;
    i_cell_cnt_array[60  ] = 165;
    i_cell_cnt_array[61  ] = 158;
    i_cell_cnt_array[62  ] = 94;
    i_cell_cnt_array[63  ] = 19;
    i_cell_cnt_array[64  ] = 20;
    i_cell_cnt_array[65  ] = 28;
    i_cell_cnt_array[66  ] = 25;
    i_cell_cnt_array[67  ] = 24;
    i_cell_cnt_array[68  ] = 23;
    i_cell_cnt_array[69  ] = 14;
    i_cell_cnt_array[70  ] = 153;
    i_cell_cnt_array[71  ] = 158;
    i_cell_cnt_array[72  ] = 161;
    i_cell_cnt_array[73  ] = 164;
    i_cell_cnt_array[74  ] = 173;
    i_cell_cnt_array[75  ] = 160;
    i_cell_cnt_array[76  ] = 92;
    i_cell_cnt_array[77  ] = 156;
    i_cell_cnt_array[78  ] = 162;
    i_cell_cnt_array[79  ] = 153;
    i_cell_cnt_array[80  ] = 147;
    i_cell_cnt_array[81  ] = 164;
    i_cell_cnt_array[82  ] = 168;
    i_cell_cnt_array[83  ] = 98;
    i_cell_cnt_array[84  ] = 154;
    i_cell_cnt_array[85  ] = 162;
    i_cell_cnt_array[86  ] = 194;
    i_cell_cnt_array[87  ] = 198;
    i_cell_cnt_array[88  ] = 169;
    i_cell_cnt_array[89  ] = 166;
    i_cell_cnt_array[90  ] = 102;
    i_cell_cnt_array[91  ] = 149;
    i_cell_cnt_array[92  ] = 180;
    i_cell_cnt_array[93  ] = 178;
    i_cell_cnt_array[94  ] = 177;
    i_cell_cnt_array[95  ] = 165;
    i_cell_cnt_array[96  ] = 159;
    i_cell_cnt_array[97  ] = 107;
    i_cell_cnt_array[98  ] = 138;
    i_cell_cnt_array[99  ] = 168;
    i_cell_cnt_array[100 ] = 179;
    i_cell_cnt_array[101 ] = 186;
    i_cell_cnt_array[102 ] = 194;
    i_cell_cnt_array[103 ] = 156;
    i_cell_cnt_array[104 ] = 102;
    i_cell_cnt_array[105 ] = 125;
    i_cell_cnt_array[106 ] = 180;
    i_cell_cnt_array[107 ] = 193;
    i_cell_cnt_array[108 ] = 186;
    i_cell_cnt_array[109 ] = 186;
    i_cell_cnt_array[110 ] = 177;
    i_cell_cnt_array[111 ] = 99;
    i_cell_cnt_array[112 ] = 149;
    i_cell_cnt_array[113 ] = 189;
    i_cell_cnt_array[114 ] = 188;
    i_cell_cnt_array[115 ] = 193;
    i_cell_cnt_array[116 ] = 190;
    i_cell_cnt_array[117 ] = 160;
    i_cell_cnt_array[118 ] = 103;
    i_cell_cnt_array[119 ] = 164;
    i_cell_cnt_array[120 ] = 145;
    i_cell_cnt_array[121 ] = 159;
    i_cell_cnt_array[122 ] = 162;
    i_cell_cnt_array[123 ] = 155;
    i_cell_cnt_array[124 ] = 171;
    i_cell_cnt_array[125 ] = 104;
    i_cell_cnt_array[126 ] = 153;
    i_cell_cnt_array[127 ] = 172;
    i_cell_cnt_array[128 ] = 177;
    i_cell_cnt_array[129 ] = 175;
    i_cell_cnt_array[130 ] = 177;
    i_cell_cnt_array[131 ] = 168;
    i_cell_cnt_array[132 ] = 85;
    i_cell_cnt_array[133 ] = 23;
    i_cell_cnt_array[134 ] = 29;
    i_cell_cnt_array[135 ] = 22;
    i_cell_cnt_array[136 ] = 19;
    i_cell_cnt_array[137 ] = 25;
    i_cell_cnt_array[138 ] = 25;
    i_cell_cnt_array[139 ] = 18;
    i_cell_cnt_array[140 ] = 142;
    i_cell_cnt_array[141 ] = 166;
    i_cell_cnt_array[142 ] = 146;
    i_cell_cnt_array[143 ] = 149;
    i_cell_cnt_array[144 ] = 155;
    i_cell_cnt_array[145 ] = 169;
    i_cell_cnt_array[146 ] = 97;
    i_cell_cnt_array[147 ] = 167;
    i_cell_cnt_array[148 ] = 175;
    i_cell_cnt_array[149 ] = 189;
    i_cell_cnt_array[150 ] = 199;
    i_cell_cnt_array[151 ] = 174;
    i_cell_cnt_array[152 ] = 164;
    i_cell_cnt_array[153 ] = 100;
    i_cell_cnt_array[154 ] = 146;
    i_cell_cnt_array[155 ] = 180;
    i_cell_cnt_array[156 ] = 182;
    i_cell_cnt_array[157 ] = 151;
    i_cell_cnt_array[158 ] = 168;
    i_cell_cnt_array[159 ] = 178;
    i_cell_cnt_array[160 ] = 98;
    i_cell_cnt_array[161 ] = 146;
    i_cell_cnt_array[162 ] = 195;
    i_cell_cnt_array[163 ] = 171;
    i_cell_cnt_array[164 ] = 173;
    i_cell_cnt_array[165 ] = 183;
    i_cell_cnt_array[166 ] = 195;
    i_cell_cnt_array[167 ] = 84;
    i_cell_cnt_array[168 ] = 133;
    i_cell_cnt_array[169 ] = 196;
    i_cell_cnt_array[170 ] = 184;
    i_cell_cnt_array[171 ] = 178;
    i_cell_cnt_array[172 ] = 187;
    i_cell_cnt_array[173 ] = 176;
    i_cell_cnt_array[174 ] = 94;
    i_cell_cnt_array[175 ] = 151;
    i_cell_cnt_array[176 ] = 202;
    i_cell_cnt_array[177 ] = 183;
    i_cell_cnt_array[178 ] = 151;
    i_cell_cnt_array[179 ] = 193;
    i_cell_cnt_array[180 ] = 191;
    i_cell_cnt_array[181 ] = 104;
    i_cell_cnt_array[182 ] = 149;
    i_cell_cnt_array[183 ] = 193;
    i_cell_cnt_array[184 ] = 167;
    i_cell_cnt_array[185 ] = 164;
    i_cell_cnt_array[186 ] = 177;
    i_cell_cnt_array[187 ] = 162;
    i_cell_cnt_array[188 ] = 116;
    i_cell_cnt_array[189 ] = 157;
    i_cell_cnt_array[190 ] = 155;
    i_cell_cnt_array[191 ] = 187;
    i_cell_cnt_array[192 ] = 198;
    i_cell_cnt_array[193 ] = 178;
    i_cell_cnt_array[194 ] = 146;
    i_cell_cnt_array[195 ] = 107;
    i_cell_cnt_array[196 ] = 165;
    i_cell_cnt_array[197 ] = 179;
    i_cell_cnt_array[198 ] = 171;
    i_cell_cnt_array[199 ] = 155;
    i_cell_cnt_array[200 ] = 168;
    i_cell_cnt_array[201 ] = 171;
    i_cell_cnt_array[202 ] = 100;
    i_cell_cnt_array[203 ] = 22;
    i_cell_cnt_array[204 ] = 23;
    i_cell_cnt_array[205 ] = 21;
    i_cell_cnt_array[206 ] = 25;
    i_cell_cnt_array[207 ] = 32;
    i_cell_cnt_array[208 ] = 23;
    i_cell_cnt_array[209 ] = 11;
    i_cell_cnt_array[210 ] = 138;
    i_cell_cnt_array[211 ] = 152;
    i_cell_cnt_array[212 ] = 155;
    i_cell_cnt_array[213 ] = 150;
    i_cell_cnt_array[214 ] = 144;
    i_cell_cnt_array[215 ] = 154;
    i_cell_cnt_array[216 ] = 102;
    i_cell_cnt_array[217 ] = 149;
    i_cell_cnt_array[218 ] = 176;
    i_cell_cnt_array[219 ] = 199;
    i_cell_cnt_array[220 ] = 192;
    i_cell_cnt_array[221 ] = 184;
    i_cell_cnt_array[222 ] = 169;
    i_cell_cnt_array[223 ] = 101;
    i_cell_cnt_array[224 ] = 140;
    i_cell_cnt_array[225 ] = 176;
    i_cell_cnt_array[226 ] = 202;
    i_cell_cnt_array[227 ] = 151;
    i_cell_cnt_array[228 ] = 176;
    i_cell_cnt_array[229 ] = 185;
    i_cell_cnt_array[230 ] = 101;
    i_cell_cnt_array[231 ] = 152;
    i_cell_cnt_array[232 ] = 177;
    i_cell_cnt_array[233 ] = 187;
    i_cell_cnt_array[234 ] = 179;
    i_cell_cnt_array[235 ] = 177;
    i_cell_cnt_array[236 ] = 194;
    i_cell_cnt_array[237 ] = 90;
    i_cell_cnt_array[238 ] = 162;
    i_cell_cnt_array[239 ] = 202;
    i_cell_cnt_array[240 ] = 195;
    i_cell_cnt_array[241 ] = 149;
    i_cell_cnt_array[242 ] = 190;
    i_cell_cnt_array[243 ] = 184;
    i_cell_cnt_array[244 ] = 84;
    i_cell_cnt_array[245 ] = 155;
    i_cell_cnt_array[246 ] = 192;
    i_cell_cnt_array[247 ] = 202;
    i_cell_cnt_array[248 ] = 189;
    i_cell_cnt_array[249 ] = 191;
    i_cell_cnt_array[250 ] = 203;
    i_cell_cnt_array[251 ] = 85;
    i_cell_cnt_array[252 ] = 138;
    i_cell_cnt_array[253 ] = 192;
    i_cell_cnt_array[254 ] = 183;
    i_cell_cnt_array[255 ] = 170;
    i_cell_cnt_array[256 ] = 174;
    i_cell_cnt_array[257 ] = 187;
    i_cell_cnt_array[258 ] = 90;
    i_cell_cnt_array[259 ] = 148;
    i_cell_cnt_array[260 ] = 175;
    i_cell_cnt_array[261 ] = 185;
    i_cell_cnt_array[262 ] = 187;
    i_cell_cnt_array[263 ] = 200;
    i_cell_cnt_array[264 ] = 167;
    i_cell_cnt_array[265 ] = 98;
    i_cell_cnt_array[266 ] = 150;
    i_cell_cnt_array[267 ] = 152;
    i_cell_cnt_array[268 ] = 154;
    i_cell_cnt_array[269 ] = 169;
    i_cell_cnt_array[270 ] = 159;
    i_cell_cnt_array[271 ] = 170;
    i_cell_cnt_array[272 ] = 101;
    i_cell_cnt_array[273 ] = 22;
    i_cell_cnt_array[274 ] = 26;
    i_cell_cnt_array[275 ] = 27;
    i_cell_cnt_array[276 ] = 22;
    i_cell_cnt_array[277 ] = 21;
    i_cell_cnt_array[278 ] = 19;
    i_cell_cnt_array[279 ] = 13;
    i_cell_cnt_array[280 ] = 151;
    i_cell_cnt_array[281 ] = 144;
    i_cell_cnt_array[282 ] = 153;
    i_cell_cnt_array[283 ] = 162;
    i_cell_cnt_array[284 ] = 144;
    i_cell_cnt_array[285 ] = 162;
    i_cell_cnt_array[286 ] = 83;
    i_cell_cnt_array[287 ] = 138;
    i_cell_cnt_array[288 ] = 174;
    i_cell_cnt_array[289 ] = 179;
    i_cell_cnt_array[290 ] = 174;
    i_cell_cnt_array[291 ] = 192;
    i_cell_cnt_array[292 ] = 160;
    i_cell_cnt_array[293 ] = 100;
    i_cell_cnt_array[294 ] = 139;
    i_cell_cnt_array[295 ] = 191;
    i_cell_cnt_array[296 ] = 188;
    i_cell_cnt_array[297 ] = 170;
    i_cell_cnt_array[298 ] = 188;
    i_cell_cnt_array[299 ] = 182;
    i_cell_cnt_array[300 ] = 100;
    i_cell_cnt_array[301 ] = 130;
    i_cell_cnt_array[302 ] = 197;
    i_cell_cnt_array[303 ] = 188;
    i_cell_cnt_array[304 ] = 182;
    i_cell_cnt_array[305 ] = 184;
    i_cell_cnt_array[306 ] = 192;
    i_cell_cnt_array[307 ] = 77;
    i_cell_cnt_array[308 ] = 150;
    i_cell_cnt_array[309 ] = 185;
    i_cell_cnt_array[310 ] = 190;
    i_cell_cnt_array[311 ] = 188;
    i_cell_cnt_array[312 ] = 193;
    i_cell_cnt_array[313 ] = 193;
    i_cell_cnt_array[314 ] = 73;
    i_cell_cnt_array[315 ] = 134;
    i_cell_cnt_array[316 ] = 197;
    i_cell_cnt_array[317 ] = 193;
    i_cell_cnt_array[318 ] = 174;
    i_cell_cnt_array[319 ] = 203;
    i_cell_cnt_array[320 ] = 191;
    i_cell_cnt_array[321 ] = 68;
    i_cell_cnt_array[322 ] = 147;
    i_cell_cnt_array[323 ] = 183;
    i_cell_cnt_array[324 ] = 189;
    i_cell_cnt_array[325 ] = 171;
    i_cell_cnt_array[326 ] = 182;
    i_cell_cnt_array[327 ] = 190;
    i_cell_cnt_array[328 ] = 87;
    i_cell_cnt_array[329 ] = 155;
    i_cell_cnt_array[330 ] = 171;
    i_cell_cnt_array[331 ] = 191;
    i_cell_cnt_array[332 ] = 176;
    i_cell_cnt_array[333 ] = 192;
    i_cell_cnt_array[334 ] = 162;
    i_cell_cnt_array[335 ] = 103;
    i_cell_cnt_array[336 ] = 150;
    i_cell_cnt_array[337 ] = 151;
    i_cell_cnt_array[338 ] = 167;
    i_cell_cnt_array[339 ] = 147;
    i_cell_cnt_array[340 ] = 159;
    i_cell_cnt_array[341 ] = 162;
    i_cell_cnt_array[342 ] = 102;
    i_cell_cnt_array[343 ] = 14;
    i_cell_cnt_array[344 ] = 26;
    i_cell_cnt_array[345 ] = 19;
    i_cell_cnt_array[346 ] = 20;
    i_cell_cnt_array[347 ] = 26;
    i_cell_cnt_array[348 ] = 23;
    i_cell_cnt_array[349 ] = 14;
    i_cell_cnt_array[350 ] = 152;
    i_cell_cnt_array[351 ] = 152;
    i_cell_cnt_array[352 ] = 143;
    i_cell_cnt_array[353 ] = 158;
    i_cell_cnt_array[354 ] = 163;
    i_cell_cnt_array[355 ] = 184;
    i_cell_cnt_array[356 ] = 92;
    i_cell_cnt_array[357 ] = 155;
    i_cell_cnt_array[358 ] = 147;
    i_cell_cnt_array[359 ] = 191;
    i_cell_cnt_array[360 ] = 175;
    i_cell_cnt_array[361 ] = 191;
    i_cell_cnt_array[362 ] = 150;
    i_cell_cnt_array[363 ] = 101;
    i_cell_cnt_array[364 ] = 141;
    i_cell_cnt_array[365 ] = 178;
    i_cell_cnt_array[366 ] = 178;
    i_cell_cnt_array[367 ] = 182;
    i_cell_cnt_array[368 ] = 179;
    i_cell_cnt_array[369 ] = 187;
    i_cell_cnt_array[370 ] = 92;
    i_cell_cnt_array[371 ] = 145;
    i_cell_cnt_array[372 ] = 204;
    i_cell_cnt_array[373 ] = 169;
    i_cell_cnt_array[374 ] = 152;
    i_cell_cnt_array[375 ] = 189;
    i_cell_cnt_array[376 ] = 171;
    i_cell_cnt_array[377 ] = 101;
    i_cell_cnt_array[378 ] = 149;
    i_cell_cnt_array[379 ] = 185;
    i_cell_cnt_array[380 ] = 210;
    i_cell_cnt_array[381 ] = 179;
    i_cell_cnt_array[382 ] = 189;
    i_cell_cnt_array[383 ] = 195;
    i_cell_cnt_array[384 ] = 77;
    i_cell_cnt_array[385 ] = 143;
    i_cell_cnt_array[386 ] = 183;
    i_cell_cnt_array[387 ] = 180;
    i_cell_cnt_array[388 ] = 160;
    i_cell_cnt_array[389 ] = 193;
    i_cell_cnt_array[390 ] = 191;
    i_cell_cnt_array[391 ] = 95;
    i_cell_cnt_array[392 ] = 132;
    i_cell_cnt_array[393 ] = 181;
    i_cell_cnt_array[394 ] = 201;
    i_cell_cnt_array[395 ] = 165;
    i_cell_cnt_array[396 ] = 154;
    i_cell_cnt_array[397 ] = 191;
    i_cell_cnt_array[398 ] = 77;
    i_cell_cnt_array[399 ] = 148;
    i_cell_cnt_array[400 ] = 183;
    i_cell_cnt_array[401 ] = 181;
    i_cell_cnt_array[402 ] = 180;
    i_cell_cnt_array[403 ] = 188;
    i_cell_cnt_array[404 ] = 157;
    i_cell_cnt_array[405 ] = 95;
    i_cell_cnt_array[406 ] = 160;
    i_cell_cnt_array[407 ] = 161;
    i_cell_cnt_array[408 ] = 170;
    i_cell_cnt_array[409 ] = 172;
    i_cell_cnt_array[410 ] = 159;
    i_cell_cnt_array[411 ] = 178;
    i_cell_cnt_array[412 ] = 95;
    i_cell_cnt_array[413 ] = 18;
    i_cell_cnt_array[414 ] = 31;
    i_cell_cnt_array[415 ] = 28;
    i_cell_cnt_array[416 ] = 21;
    i_cell_cnt_array[417 ] = 27;
    i_cell_cnt_array[418 ] = 24;
    i_cell_cnt_array[419 ] = 15;
    i_cell_cnt_array[420 ] = 151;
    i_cell_cnt_array[421 ] = 168;
    i_cell_cnt_array[422 ] = 145;
    i_cell_cnt_array[423 ] = 140;
    i_cell_cnt_array[424 ] = 165;
    i_cell_cnt_array[425 ] = 165;
    i_cell_cnt_array[426 ] = 90;
    i_cell_cnt_array[427 ] = 135;
    i_cell_cnt_array[428 ] = 158;
    i_cell_cnt_array[429 ] = 179;
    i_cell_cnt_array[430 ] = 202;
    i_cell_cnt_array[431 ] = 186;
    i_cell_cnt_array[432 ] = 157;
    i_cell_cnt_array[433 ] = 99;
    i_cell_cnt_array[434 ] = 152;
    i_cell_cnt_array[435 ] = 199;
    i_cell_cnt_array[436 ] = 176;
    i_cell_cnt_array[437 ] = 148;
    i_cell_cnt_array[438 ] = 188;
    i_cell_cnt_array[439 ] = 151;
    i_cell_cnt_array[440 ] = 103;
    i_cell_cnt_array[441 ] = 145;
    i_cell_cnt_array[442 ] = 191;
    i_cell_cnt_array[443 ] = 181;
    i_cell_cnt_array[444 ] = 187;
    i_cell_cnt_array[445 ] = 195;
    i_cell_cnt_array[446 ] = 173;
    i_cell_cnt_array[447 ] = 111;
    i_cell_cnt_array[448 ] = 125;
    i_cell_cnt_array[449 ] = 194;
    i_cell_cnt_array[450 ] = 189;
    i_cell_cnt_array[451 ] = 169;
    i_cell_cnt_array[452 ] = 175;
    i_cell_cnt_array[453 ] = 188;
    i_cell_cnt_array[454 ] = 100;
    i_cell_cnt_array[455 ] = 139;
    i_cell_cnt_array[456 ] = 177;
    i_cell_cnt_array[457 ] = 197;
    i_cell_cnt_array[458 ] = 169;
    i_cell_cnt_array[459 ] = 186;
    i_cell_cnt_array[460 ] = 185;
    i_cell_cnt_array[461 ] = 95;
    i_cell_cnt_array[462 ] = 156;
    i_cell_cnt_array[463 ] = 192;
    i_cell_cnt_array[464 ] = 194;
    i_cell_cnt_array[465 ] = 175;
    i_cell_cnt_array[466 ] = 166;
    i_cell_cnt_array[467 ] = 179;
    i_cell_cnt_array[468 ] = 100;
    i_cell_cnt_array[469 ] = 146;
    i_cell_cnt_array[470 ] = 183;
    i_cell_cnt_array[471 ] = 167;
    i_cell_cnt_array[472 ] = 179;
    i_cell_cnt_array[473 ] = 191;
    i_cell_cnt_array[474 ] = 153;
    i_cell_cnt_array[475 ] = 110;
    i_cell_cnt_array[476 ] = 152;
    i_cell_cnt_array[477 ] = 166;
    i_cell_cnt_array[478 ] = 172;
    i_cell_cnt_array[479 ] = 165;
    i_cell_cnt_array[480 ] = 152;
    i_cell_cnt_array[481 ] = 176;
    i_cell_cnt_array[482 ] = 97;
    i_cell_cnt_array[483 ] = 25;
    i_cell_cnt_array[484 ] = 21;
    i_cell_cnt_array[485 ] = 33;
    i_cell_cnt_array[486 ] = 31;
    i_cell_cnt_array[487 ] = 27;
    i_cell_cnt_array[488 ] = 26;
    i_cell_cnt_array[489 ] = 16;
    i_cell_cnt_array[490 ] = 146;
    i_cell_cnt_array[491 ] = 162;
    i_cell_cnt_array[492 ] = 145;
    i_cell_cnt_array[493 ] = 160;
    i_cell_cnt_array[494 ] = 160;
    i_cell_cnt_array[495 ] = 157;
    i_cell_cnt_array[496 ] = 95;
    i_cell_cnt_array[497 ] = 149;
    i_cell_cnt_array[498 ] = 163;
    i_cell_cnt_array[499 ] = 173;
    i_cell_cnt_array[500 ] = 177;
    i_cell_cnt_array[501 ] = 149;
    i_cell_cnt_array[502 ] = 174;
    i_cell_cnt_array[503 ] = 93;
    i_cell_cnt_array[504 ] = 138;
    i_cell_cnt_array[505 ] = 204;
    i_cell_cnt_array[506 ] = 188;
    i_cell_cnt_array[507 ] = 186;
    i_cell_cnt_array[508 ] = 164;
    i_cell_cnt_array[509 ] = 174;
    i_cell_cnt_array[510 ] = 107;
    i_cell_cnt_array[511 ] = 136;
    i_cell_cnt_array[512 ] = 197;
    i_cell_cnt_array[513 ] = 180;
    i_cell_cnt_array[514 ] = 174;
    i_cell_cnt_array[515 ] = 188;
    i_cell_cnt_array[516 ] = 160;
    i_cell_cnt_array[517 ] = 108;
    i_cell_cnt_array[518 ] = 121;
    i_cell_cnt_array[519 ] = 187;
    i_cell_cnt_array[520 ] = 181;
    i_cell_cnt_array[521 ] = 165;
    i_cell_cnt_array[522 ] = 165;
    i_cell_cnt_array[523 ] = 170;
    i_cell_cnt_array[524 ] = 97;
    i_cell_cnt_array[525 ] = 154;
    i_cell_cnt_array[526 ] = 174;
    i_cell_cnt_array[527 ] = 176;
    i_cell_cnt_array[528 ] = 174;
    i_cell_cnt_array[529 ] = 186;
    i_cell_cnt_array[530 ] = 168;
    i_cell_cnt_array[531 ] = 86;
    i_cell_cnt_array[532 ] = 156;
    i_cell_cnt_array[533 ] = 165;
    i_cell_cnt_array[534 ] = 183;
    i_cell_cnt_array[535 ] = 190;
    i_cell_cnt_array[536 ] = 202;
    i_cell_cnt_array[537 ] = 168;
    i_cell_cnt_array[538 ] = 108;
    i_cell_cnt_array[539 ] = 166;
    i_cell_cnt_array[540 ] = 157;
    i_cell_cnt_array[541 ] = 147;
    i_cell_cnt_array[542 ] = 165;
    i_cell_cnt_array[543 ] = 174;
    i_cell_cnt_array[544 ] = 151;
    i_cell_cnt_array[545 ] = 102;
    i_cell_cnt_array[546 ] = 152;
    i_cell_cnt_array[547 ] = 166;
    i_cell_cnt_array[548 ] = 179;
    i_cell_cnt_array[549 ] = 162;
    i_cell_cnt_array[550 ] = 154;
    i_cell_cnt_array[551 ] = 173;
    i_cell_cnt_array[552 ] = 97;
    i_cell_cnt_array[553 ] = 16;
    i_cell_cnt_array[554 ] = 23;
    i_cell_cnt_array[555 ] = 27;
    i_cell_cnt_array[556 ] = 21;
    i_cell_cnt_array[557 ] = 25;
    i_cell_cnt_array[558 ] = 20;
    i_cell_cnt_array[559 ] = 16;
    i_cell_cnt_array[560 ] = 152;
    i_cell_cnt_array[561 ] = 155;
    i_cell_cnt_array[562 ] = 179;
    i_cell_cnt_array[563 ] = 156;
    i_cell_cnt_array[564 ] = 163;
    i_cell_cnt_array[565 ] = 169;
    i_cell_cnt_array[566 ] = 98;
    i_cell_cnt_array[567 ] = 159;
    i_cell_cnt_array[568 ] = 157;
    i_cell_cnt_array[569 ] = 169;
    i_cell_cnt_array[570 ] = 158;
    i_cell_cnt_array[571 ] = 170;
    i_cell_cnt_array[572 ] = 176;
    i_cell_cnt_array[573 ] = 93;
    i_cell_cnt_array[574 ] = 159;
    i_cell_cnt_array[575 ] = 172;
    i_cell_cnt_array[576 ] = 169;
    i_cell_cnt_array[577 ] = 148;
    i_cell_cnt_array[578 ] = 167;
    i_cell_cnt_array[579 ] = 179;
    i_cell_cnt_array[580 ] = 106;
    i_cell_cnt_array[581 ] = 155;
    i_cell_cnt_array[582 ] = 152;
    i_cell_cnt_array[583 ] = 164;
    i_cell_cnt_array[584 ] = 172;
    i_cell_cnt_array[585 ] = 157;
    i_cell_cnt_array[586 ] = 161;
    i_cell_cnt_array[587 ] = 101;
    i_cell_cnt_array[588 ] = 155;
    i_cell_cnt_array[589 ] = 169;
    i_cell_cnt_array[590 ] = 162;
    i_cell_cnt_array[591 ] = 168;
    i_cell_cnt_array[592 ] = 155;
    i_cell_cnt_array[593 ] = 167;
    i_cell_cnt_array[594 ] = 107;
    i_cell_cnt_array[595 ] = 148;
    i_cell_cnt_array[596 ] = 160;
    i_cell_cnt_array[597 ] = 154;
    i_cell_cnt_array[598 ] = 158;
    i_cell_cnt_array[599 ] = 151;
    i_cell_cnt_array[600 ] = 150;
    i_cell_cnt_array[601 ] = 107;
    i_cell_cnt_array[602 ] = 153;
    i_cell_cnt_array[603 ] = 165;
    i_cell_cnt_array[604 ] = 167;
    i_cell_cnt_array[605 ] = 168;
    i_cell_cnt_array[606 ] = 154;
    i_cell_cnt_array[607 ] = 176;
    i_cell_cnt_array[608 ] = 107;
    i_cell_cnt_array[609 ] = 165;
    i_cell_cnt_array[610 ] = 177;
    i_cell_cnt_array[611 ] = 178;
    i_cell_cnt_array[612 ] = 180;
    i_cell_cnt_array[613 ] = 180;
    i_cell_cnt_array[614 ] = 183;
    i_cell_cnt_array[615 ] = 109;
    i_cell_cnt_array[616 ] = 159;
    i_cell_cnt_array[617 ] = 172;
    i_cell_cnt_array[618 ] = 180;
    i_cell_cnt_array[619 ] = 160;
    i_cell_cnt_array[620 ] = 168;
    i_cell_cnt_array[621 ] = 172;
    i_cell_cnt_array[622 ] = 92;
    i_cell_cnt_array[623 ] = 21;
    i_cell_cnt_array[624 ] = 22;
    i_cell_cnt_array[625 ] = 24;
    i_cell_cnt_array[626 ] = 28;
    i_cell_cnt_array[627 ] = 21;
    i_cell_cnt_array[628 ] = 23;
    i_cell_cnt_array[629 ] = 15;
    i_cell_cnt_array[630 ] = 21;
    i_cell_cnt_array[631 ] = 19;
    i_cell_cnt_array[632 ] = 14;
    i_cell_cnt_array[633 ] = 21;
    i_cell_cnt_array[634 ] = 16;
    i_cell_cnt_array[635 ] = 24;
    i_cell_cnt_array[636 ] = 12;
    i_cell_cnt_array[637 ] = 23;
    i_cell_cnt_array[638 ] = 19;
    i_cell_cnt_array[639 ] = 13;
    i_cell_cnt_array[640 ] = 26;
    i_cell_cnt_array[641 ] = 16;
    i_cell_cnt_array[642 ] = 18;
    i_cell_cnt_array[643 ] = 15;
    i_cell_cnt_array[644 ] = 23;
    i_cell_cnt_array[645 ] = 24;
    i_cell_cnt_array[646 ] = 22;
    i_cell_cnt_array[647 ] = 20;
    i_cell_cnt_array[648 ] = 18;
    i_cell_cnt_array[649 ] = 17;
    i_cell_cnt_array[650 ] = 13;
    i_cell_cnt_array[651 ] = 19;
    i_cell_cnt_array[652 ] = 20;
    i_cell_cnt_array[653 ] = 19;
    i_cell_cnt_array[654 ] = 22;
    i_cell_cnt_array[655 ] = 19;
    i_cell_cnt_array[656 ] = 24;
    i_cell_cnt_array[657 ] = 15;
    i_cell_cnt_array[658 ] = 25;
    i_cell_cnt_array[659 ] = 16;
    i_cell_cnt_array[660 ] = 18;
    i_cell_cnt_array[661 ] = 24;
    i_cell_cnt_array[662 ] = 20;
    i_cell_cnt_array[663 ] = 26;
    i_cell_cnt_array[664 ] = 13;
    i_cell_cnt_array[665 ] = 23;
    i_cell_cnt_array[666 ] = 24;
    i_cell_cnt_array[667 ] = 21;
    i_cell_cnt_array[668 ] = 17;
    i_cell_cnt_array[669 ] = 26;
    i_cell_cnt_array[670 ] = 16;
    i_cell_cnt_array[671 ] = 13;
    i_cell_cnt_array[672 ] = 16;
    i_cell_cnt_array[673 ] = 22;
    i_cell_cnt_array[674 ] = 17;
    i_cell_cnt_array[675 ] = 25;
    i_cell_cnt_array[676 ] = 26;
    i_cell_cnt_array[677 ] = 24;
    i_cell_cnt_array[678 ] = 14;
    i_cell_cnt_array[679 ] = 21;
    i_cell_cnt_array[680 ] = 12;
    i_cell_cnt_array[681 ] = 22;
    i_cell_cnt_array[682 ] = 20;
    i_cell_cnt_array[683 ] = 19;
    i_cell_cnt_array[684 ] = 18;
    i_cell_cnt_array[685 ] = 11;
    i_cell_cnt_array[686 ] = 20;
    i_cell_cnt_array[687 ] = 26;
    i_cell_cnt_array[688 ] = 24;
    i_cell_cnt_array[689 ] = 24;
    i_cell_cnt_array[690 ] = 17;
    i_cell_cnt_array[691 ] = 28;
    i_cell_cnt_array[692 ] = 14;
    i_cell_cnt_array[693 ] = 3;
    i_cell_cnt_array[694 ] = 2;
    i_cell_cnt_array[695 ] = 3;
    i_cell_cnt_array[696 ] = 4;
    i_cell_cnt_array[697 ] = 4;
    i_cell_cnt_array[698 ] = 3;
    i_cell_cnt_array[699 ] = 3;

    return i_cell_cnt_array[cell_index];
}


//	implementation of my_erfc_table
float my_erfc (float input)
{
	
	float erfc_table_left[300];
	float erfc_table_right[300];

	erfc_table_left   [0  	 	] = 0.77729743719100952148437500000000000000000000000000;
	erfc_table_left   [1  	 	] = 0.76647806167602539062500000000000000000000000000000;
	erfc_table_left   [2  	 	] = 0.75570410490036010742187500000000000000000000000000;
	erfc_table_left   [3  	 	] = 0.74497741460800170898437500000000000000000000000000;
	erfc_table_left   [4  	 	] = 0.73429995775222778320312500000000000000000000000000;
	erfc_table_left   [5  	 	] = 0.72367358207702636718750000000000000000000000000000;
	erfc_table_left   [6  	 	] = 0.71310025453567504882812500000000000000000000000000;
	erfc_table_left   [7  	 	] = 0.70258176326751708984375000000000000000000000000000;
	erfc_table_left   [8  	 	] = 0.69211995601654052734375000000000000000000000000000;
	erfc_table_left   [9  	 	] = 0.68171650171279907226562500000000000000000000000000;
	erfc_table_left   [10   	] = 0.67137324810028076171875000000000000000000000000000;
	erfc_table_left   [11   	] = 0.66109186410903930664062500000000000000000000000000;
	erfc_table_left   [12   	] = 0.65087401866912841796875000000000000000000000000000;
	erfc_table_left   [13   	] = 0.64072132110595703125000000000000000000000000000000;
	erfc_table_left   [14   	] = 0.63063544034957885742187500000000000000000000000000;
	erfc_table_left   [15   	] = 0.62061792612075805664062500000000000000000000000000;
	erfc_table_left   [16   	] = 0.61067032814025878906250000000000000000000000000000;
	erfc_table_left   [17   	] = 0.60079401731491088867187500000000000000000000000000;
	erfc_table_left   [18   	] = 0.59099054336547851562500000000000000000000000000000;
	erfc_table_left   [19   	] = 0.58126127719879150390625000000000000000000000000000;
	erfc_table_left   [20   	] = 0.57160764932632446289062500000000000000000000000000;
	erfc_table_left   [21   	] = 0.56203091144561767578125000000000000000000000000000;
	erfc_table_left   [22   	] = 0.55253237485885620117187500000000000000000000000000;
	erfc_table_left   [23   	] = 0.54311329126358032226562500000000000000000000000000;
	erfc_table_left   [24   	] = 0.53377491235733032226562500000000000000000000000000;
	erfc_table_left   [25   	] = 0.52451825141906738281250000000000000000000000000000;
	erfc_table_left   [26   	] = 0.51534461975097656250000000000000000000000000000000;
	erfc_table_left   [27   	] = 0.50625491142272949218750000000000000000000000000000;
	erfc_table_left   [28   	] = 0.49725031852722167968750000000000000000000000000000;
	erfc_table_left   [29   	] = 0.48833173513412475585937500000000000000000000000000;
	erfc_table_left   [30   	] = 0.47950011491775512695312500000000000000000000000000;
	erfc_table_left   [31   	] = 0.47075638175010681152343750000000000000000000000000;
	erfc_table_left   [32   	] = 0.46210137009620666503906250000000000000000000000000;
	erfc_table_left   [33   	] = 0.45353588461875915527343750000000000000000000000000;
	erfc_table_left   [34   	] = 0.44506075978279113769531250000000000000000000000000;
	erfc_table_left   [35   	] = 0.43667665123939514160156250000000000000000000000000;
	erfc_table_left   [36   	] = 0.42838421463966369628906250000000000000000000000000;
	erfc_table_left   [37   	] = 0.42018419504165649414062500000000000000000000000000;
	erfc_table_left   [38   	] = 0.41207709908485412597656250000000000000000000000000;
	erfc_table_left   [39   	] = 0.40406352281570434570312500000000000000000000000000;
	erfc_table_left   [40   	] = 0.39614391326904296875000000000000000000000000000000;
	erfc_table_left   [41   	] = 0.38831877708435058593750000000000000000000000000000;
	erfc_table_left   [42   	] = 0.38058856129646301269531250000000000000000000000000;
	erfc_table_left   [43   	] = 0.37295356392860412597656250000000000000000000000000;
	erfc_table_left   [44   	] = 0.36541417241096496582031250000000000000000000000000;
	erfc_table_left   [45   	] = 0.35797068476676940917968750000000000000000000000000;
	erfc_table_left   [46   	] = 0.35062330961227416992187500000000000000000000000000;
	erfc_table_left   [47   	] = 0.34337228536605834960937500000000000000000000000000;
	erfc_table_left   [48   	] = 0.33621779084205627441406250000000000000000000000000;
	erfc_table_left   [49   	] = 0.32915994524955749511718750000000000000000000000000;
	erfc_table_left   [50   	] = 0.32219880819320678710937500000000000000000000000000;
	erfc_table_left   [51   	] = 0.31533446907997131347656250000000000000000000000000;
	erfc_table_left   [52   	] = 0.30856689810752868652343750000000000000000000000000;
	erfc_table_left   [53   	] = 0.30189606547355651855468750000000000000000000000000;
	erfc_table_left   [54   	] = 0.29532191157341003417968750000000000000000000000000;
	erfc_table_left   [55   	] = 0.28884434700012207031250000000000000000000000000000;
	erfc_table_left   [56   	] = 0.28246325254440307617187500000000000000000000000000;
	erfc_table_left   [57   	] = 0.27617838978767395019531250000000000000000000000000;
	erfc_table_left   [58   	] = 0.26998957991600036621093750000000000000000000000000;
	erfc_table_left   [59   	] = 0.26389655470848083496093750000000000000000000000000;
	erfc_table_left   [60   	] = 0.25789901614189147949218750000000000000000000000000;
	erfc_table_left   [61   	] = 0.25199672579765319824218750000000000000000000000000;
	erfc_table_left   [62   	] = 0.24618925154209136962890625000000000000000000000000;
	erfc_table_left   [63   	] = 0.24047625064849853515625000000000000000000000000000;
	erfc_table_left   [64   	] = 0.23485729098320007324218750000000000000000000000000;
	erfc_table_left   [65   	] = 0.22933195531368255615234375000000000000000000000000;
	erfc_table_left   [66   	] = 0.22389972209930419921875000000000000000000000000000;
	erfc_table_left   [67   	] = 0.21856014430522918701171875000000000000000000000000;
	erfc_table_left   [68   	] = 0.21331267058849334716796875000000000000000000000000;
	erfc_table_left   [69   	] = 0.20815674960613250732421875000000000000000000000000;
	erfc_table_left   [70   	] = 0.20309180021286010742187500000000000000000000000000;
	erfc_table_left   [71   	] = 0.19811718165874481201171875000000000000000000000000;
	erfc_table_left   [72   	] = 0.19323226809501647949218750000000000000000000000000;
	erfc_table_left   [73   	] = 0.18843643367290496826171875000000000000000000000000;
	erfc_table_left   [74   	] = 0.18372897803783416748046875000000000000000000000000;
	erfc_table_left   [75   	] = 0.17910918593406677246093750000000000000000000000000;
	erfc_table_left   [76   	] = 0.17457635700702667236328125000000000000000000000000;
	erfc_table_left   [77   	] = 0.17012971639633178710937500000000000000000000000000;
	erfc_table_left   [78   	] = 0.16576850414276123046875000000000000000000000000000;
	erfc_table_left   [79   	] = 0.16149191558361053466796875000000000000000000000000;
	erfc_table_left   [80   	] = 0.15729920566082000732421875000000000000000000000000;
	erfc_table_left   [81   	] = 0.15318951010704040527343750000000000000000000000000;
	erfc_table_left   [82   	] = 0.14916197955608367919921875000000000000000000000000;
	erfc_table_left   [83   	] = 0.14521579444408416748046875000000000000000000000000;
	erfc_table_left   [84   	] = 0.14135006070137023925781250000000000000000000000000;
	erfc_table_left   [85   	] = 0.13756388425827026367187500000000000000000000000000;
	erfc_table_left   [86   	] = 0.13385640084743499755859375000000000000000000000000;
	erfc_table_left   [87   	] = 0.13022670149803161621093750000000000000000000000000;
	erfc_table_left   [88   	] = 0.12667384743690490722656250000000000000000000000000;
	erfc_table_left   [89   	] = 0.12319689989089965820312500000000000000000000000000;
	erfc_table_left   [90   	] = 0.11979493498802185058593750000000000000000000000000;
	erfc_table_left   [91   	] = 0.11646697670221328735351562500000000000000000000000;
	erfc_table_left   [92   	] = 0.11321210116147994995117187500000000000000000000000;
	erfc_table_left   [93   	] = 0.11002932488918304443359375000000000000000000000000;
	erfc_table_left   [94   	] = 0.10691767185926437377929687500000000000000000000000;
	erfc_table_left   [95   	] = 0.10387615859508514404296875000000000000000000000000;
	erfc_table_left   [96   	] = 0.10090380162000656127929687500000000000000000000000;
	erfc_table_left   [97   	] = 0.09799959510564804077148437500000000000000000000000;
	erfc_table_left   [98   	] = 0.09516257047653198242187500000000000000000000000000;
	erfc_table_left   [99   	] = 0.09239171445369720458984375000000000000000000000000;
	erfc_table_left   [100   	] = 0.08968602120876312255859375000000000000000000000000;
	erfc_table_left   [101   	] = 0.08704449236392974853515625000000000000000000000000;
	erfc_table_left   [102   	] = 0.08446612209081649780273437500000000000000000000000;
	erfc_table_left   [103   	] = 0.08194990456104278564453125000000000000000000000000;
	erfc_table_left   [104   	] = 0.07949482649564743041992187500000000000000000000000;
	erfc_table_left   [105   	] = 0.07709988206624984741210937500000000000000000000000;
	erfc_table_left   [106   	] = 0.07476407289505004882812500000000000000000000000000;
	erfc_table_left   [107   	] = 0.07248635590076446533203125000000000000000000000000;
	erfc_table_left   [108   	] = 0.07026579976081848144531250000000000000000000000000;
	erfc_table_left   [109   	] = 0.06810136139392852783203125000000000000000000000000;
	erfc_table_left   [110   	] = 0.06599204987287521362304687500000000000000000000000;
	erfc_table_left   [111   	] = 0.06393687427043914794921875000000000000000000000000;
	erfc_table_left   [112   	] = 0.06193484365940093994140625000000000000000000000000;
	erfc_table_left   [113   	] = 0.05998497456312179565429687500000000000000000000000;
	erfc_table_left   [114   	] = 0.05808628723025321960449218750000000000000000000000;
	erfc_table_left   [115   	] = 0.05623780936002731323242187500000000000000000000000;
	erfc_table_left   [116   	] = 0.05443856865167617797851562500000000000000000000000;
	erfc_table_left   [117   	] = 0.05268760770559310913085937500000000000000000000000;
	erfc_table_left   [118   	] = 0.05098397284746170043945312500000000000000000000000;
	erfc_table_left   [119   	] = 0.04932669550180435180664062500000000000000000000000;
	erfc_table_left   [120   	] = 0.04771487414836883544921875000000000000000000000000;
	erfc_table_left   [121   	] = 0.04614755511283874511718750000000000000000000000000;
	erfc_table_left   [122   	] = 0.04462381824851036071777343750000000000000000000000;
	erfc_table_left   [123   	] = 0.04314274340867996215820312500000000000000000000000;
	erfc_table_left   [124   	] = 0.04170342907309532165527343750000000000000000000000;
	erfc_table_left   [125   	] = 0.04030497372150421142578125000000000000000000000000;
	erfc_table_left   [126   	] = 0.03894649073481559753417968750000000000000000000000;
	erfc_table_left   [127   	] = 0.03762710094451904296875000000000000000000000000000;
	erfc_table_left   [128   	] = 0.03634593635797500610351562500000000000000000000000;
	erfc_table_left   [129   	] = 0.03510214015841484069824218750000000000000000000000;
	erfc_table_left   [130   	] = 0.03389485925436019897460937500000000000000000000000;
	erfc_table_left   [131   	] = 0.03272325918078422546386718750000000000000000000000;
	erfc_table_left   [132   	] = 0.03158649802207946777343750000000000000000000000000;
	erfc_table_left   [133   	] = 0.03048378601670265197753906250000000000000000000000;
	erfc_table_left   [134   	] = 0.02941430732607841491699218750000000000000000000000;
	erfc_table_left   [135   	] = 0.02837726473808288574218750000000000000000000000000;
	erfc_table_left   [136   	] = 0.02737187594175338745117187500000000000000000000000;
	erfc_table_left   [137   	] = 0.02639737166464328765869140625000000000000000000000;
	erfc_table_left   [138   	] = 0.02545299194753170013427734375000000000000000000000;
	erfc_table_left   [139   	] = 0.02453798614442348480224609375000000000000000000000;
	erfc_table_left   [140   	] = 0.02365161851048469543457031250000000000000000000000;
	erfc_table_left   [141   	] = 0.02279316633939743041992187500000000000000000000000;
	erfc_table_left   [142   	] = 0.02196191437542438507080078125000000000000000000000;
	erfc_table_left   [143   	] = 0.02115716412663459777832031250000000000000000000000;
	erfc_table_left   [144   	] = 0.02037821523845195770263671875000000000000000000000;
	erfc_table_left   [145   	] = 0.01962441205978393554687500000000000000000000000000;
	erfc_table_left   [146   	] = 0.01889507658779621124267578125000000000000000000000;
	erfc_table_left   [147   	] = 0.01818955689668655395507812500000000000000000000000;
	erfc_table_left   [148   	] = 0.01750721223652362823486328125000000000000000000000;
	erfc_table_left   [149   	] = 0.01684741303324699401855468750000000000000000000000;
	erfc_table_left   [150   	] = 0.01620954088866710662841796875000000000000000000000;
	erfc_table_left   [151   	] = 0.01559299323707818984985351562500000000000000000000;
	erfc_table_left   [152   	] = 0.01499717403203248977661132812500000000000000000000;
	erfc_table_left   [153   	] = 0.01442150212824344635009765625000000000000000000000;
	erfc_table_left   [154   	] = 0.01386540662497282028198242187500000000000000000000;
	erfc_table_left   [155   	] = 0.01332833152264356613159179687500000000000000000000;
	erfc_table_left   [156   	] = 0.01280972734093666076660156250000000000000000000000;
	erfc_table_left   [157   	] = 0.01230905484408140182495117187500000000000000000000;
	erfc_table_left   [158   	] = 0.01182580180466175079345703125000000000000000000000;
	erfc_table_left   [159   	] = 0.01135944947600364685058593750000000000000000000000;
	erfc_table_left   [160   	] = 0.01090949773788452148437500000000000000000000000000;
	erfc_table_left   [161   	] = 0.01047545485198497772216796875000000000000000000000;
	erfc_table_left   [162   	] = 0.01005684304982423782348632812500000000000000000000;
	erfc_table_left   [163   	] = 0.00965319480746984481811523437500000000000000000000;
	erfc_table_left   [164   	] = 0.00926405284553766250610351562500000000000000000000;
	erfc_table_left   [165   	] = 0.00888897106051445007324218750000000000000000000000;
	erfc_table_left   [166   	] = 0.00852751266211271286010742187500000000000000000000;
	erfc_table_left   [167   	] = 0.00817925389856100082397460937500000000000000000000;
	erfc_table_left   [168   	] = 0.00784377939999103546142578125000000000000000000000;
	erfc_table_left   [169   	] = 0.00752067985013127326965332031250000000000000000000;
	erfc_table_left   [170   	] = 0.00720956921577453613281250000000000000000000000000;
	erfc_table_left   [171   	] = 0.00691005913540720939636230468750000000000000000000;
	erfc_table_left   [172   	] = 0.00662177428603172302246093750000000000000000000000;
	erfc_table_left   [173   	] = 0.00634434912353754043579101562500000000000000000000;
	erfc_table_left   [174   	] = 0.00607742881402373313903808593750000000000000000000;
	erfc_table_left   [175   	] = 0.00582066643983125686645507812500000000000000000000;
	erfc_table_left   [176   	] = 0.00557372486218810081481933593750000000000000000000;
	erfc_table_left   [177   	] = 0.00533627578988671302795410156250000000000000000000;
	erfc_table_left   [178   	] = 0.00510800024494528770446777343750000000000000000000;
	erfc_table_left   [179   	] = 0.00488858763128519058227539062500000000000000000000;
	erfc_table_left   [180   	] = 0.00467773573473095893859863281250000000000000000000;
	erfc_table_left   [181   	] = 0.00447515165433287620544433593750000000000000000000;
	erfc_table_left   [182   	] = 0.00428054761141538619995117187500000000000000000000;
	erfc_table_left   [183   	] = 0.00409365072846412658691406250000000000000000000000;
	erfc_table_left   [184   	] = 0.00391418999060988426208496093750000000000000000000;
	erfc_table_left   [185   	] = 0.00374190346337854862213134765625000000000000000000;
	erfc_table_left   [186   	] = 0.00357653805986046791076660156250000000000000000000;
	erfc_table_left   [187   	] = 0.00341784697957336902618408203125000000000000000000;
	erfc_table_left   [188   	] = 0.00326559133827686309814453125000000000000000000000;
	erfc_table_left   [189   	] = 0.00311953970231115818023681640625000000000000000000;
	erfc_table_left   [190   	] = 0.00297946692444384098052978515625000000000000000000;
	erfc_table_left   [191   	] = 0.00284515530802309513092041015625000000000000000000;
	erfc_table_left   [192   	] = 0.00271639367565512657165527343750000000000000000000;
	erfc_table_left   [193   	] = 0.00259297736920416355133056640625000000000000000000;
	erfc_table_left   [194   	] = 0.00247470661997795104980468750000000000000000000000;
	erfc_table_left   [195   	] = 0.00236139236949384212493896484375000000000000000000;
	erfc_table_left   [196   	] = 0.00225284742191433906555175781250000000000000000000;
	erfc_table_left   [197   	] = 0.00214889156632125377655029296875000000000000000000;
	erfc_table_left   [198   	] = 0.00204935064539313316345214843750000000000000000000;
	erfc_table_left   [199   	] = 0.00195405655540525913238525390625000000000000000000;
	erfc_table_left   [200   	] = 0.00186284631490707397460937500000000000000000000000;
	erfc_table_left   [201   	] = 0.00177556206472218036651611328125000000000000000000;
	erfc_table_left   [202   	] = 0.00169205176644027233123779296875000000000000000000;
	erfc_table_left   [203   	] = 0.00161216815467923879623413085937500000000000000000;
	erfc_table_left   [204   	] = 0.00153576896991580724716186523437500000000000000000;
	erfc_table_left   [205   	] = 0.00146271695848554372787475585937500000000000000000;
	erfc_table_left   [206   	] = 0.00139287929050624370574951171875000000000000000000;
	erfc_table_left   [207   	] = 0.00132612790912389755249023437500000000000000000000;
	erfc_table_left   [208   	] = 0.00126233929768204689025878906250000000000000000000;
	erfc_table_left   [209   	] = 0.00120139413047581911087036132812500000000000000000;
	erfc_table_left   [210   	] = 0.00114317715633660554885864257812500000000000000000;
	erfc_table_left   [211   	] = 0.00108757743146270513534545898437500000000000000000;
	erfc_table_left   [212   	] = 0.00103448797017335891723632812500000000000000000000;
	erfc_table_left   [213   	] = 0.00098380446434020996093750000000000000000000000000;
	erfc_table_left   [214   	] = 0.00093542964896187186241149902343750000000000000000;
	erfc_table_left   [215   	] = 0.00088926660828292369842529296875000000000000000000;
	erfc_table_left   [216   	] = 0.00084522296674549579620361328125000000000000000000;
	erfc_table_left   [217   	] = 0.00080320989945903420448303222656250000000000000000;
	erfc_table_left   [218   	] = 0.00076314172474667429924011230468750000000000000000;
	erfc_table_left   [219   	] = 0.00072493613697588443756103515625000000000000000000;
	erfc_table_left   [220   	] = 0.00068851374089717864990234375000000000000000000000;
	erfc_table_left   [221   	] = 0.00065379828447476029396057128906250000000000000000;
	erfc_table_left   [222   	] = 0.00062071642605587840080261230468750000000000000000;
	erfc_table_left   [223   	] = 0.00058919755974784493446350097656250000000000000000;
	erfc_table_left   [224   	] = 0.00055917387362569570541381835937500000000000000000;
	erfc_table_left   [225   	] = 0.00053058011690154671669006347656250000000000000000;
	erfc_table_left   [226   	] = 0.00050335365813225507736206054687500000000000000000;
	erfc_table_left   [227   	] = 0.00047743431059643626213073730468750000000000000000;
	erfc_table_left   [228   	] = 0.00045276424498297274112701416015625000000000000000;
	erfc_table_left   [229   	] = 0.00042928796028718352317810058593750000000000000000;
	erfc_table_left   [230   	] = 0.00040695210918784141540527343750000000000000000000;
	erfc_table_left   [231   	] = 0.00038570561446249485015869140625000000000000000000;
	erfc_table_left   [232   	] = 0.00036549931974150240421295166015625000000000000000;
	erfc_table_left   [233   	] = 0.00034628616413101553916931152343750000000000000000;
	erfc_table_left   [234   	] = 0.00032802097848616540431976318359375000000000000000;
	erfc_table_left   [235   	] = 0.00031066051451489329338073730468750000000000000000;
	erfc_table_left   [236   	] = 0.00029416318284347653388977050781250000000000000000;
	erfc_table_left   [237   	] = 0.00027848925674334168434143066406250000000000000000;
	erfc_table_left   [238   	] = 0.00026360026095062494277954101562500000000000000000;
	erfc_table_left   [239   	] = 0.00024946039775386452674865722656250000000000000000;
	erfc_table_left   [240   	] = 0.00023603429144714027643203735351562500000000000000;
	erfc_table_left   [241   	] = 0.00022328850172925740480422973632812500000000000000;
	erfc_table_left   [242   	] = 0.00021119097073096781969070434570312500000000000000;
	erfc_table_left   [243   	] = 0.00019971100846305489540100097656250000000000000000;
	erfc_table_left   [244   	] = 0.00018881927826441824436187744140625000000000000000;
	erfc_table_left   [245   	] = 0.00017848770949058234691619873046875000000000000000;
	erfc_table_left   [246   	] = 0.00016868943930603563785552978515625000000000000000;
	erfc_table_left   [247   	] = 0.00015939879813231527805328369140625000000000000000;
	erfc_table_left   [248   	] = 0.00015059126599226146936416625976562500000000000000;
	erfc_table_left   [249   	] = 0.00014224338519852608442306518554687500000000000000;
	erfc_table_left   [250   	] = 0.00013433274580165743827819824218750000000000000000;
	erfc_table_left   [251   	] = 0.00012683792738243937492370605468750000000000000000;
	erfc_table_left   [252   	] = 0.00011973852815572172403335571289062500000000000000;
	erfc_table_left   [253   	] = 0.00011301500489935278892517089843750000000000000000;
	erfc_table_left   [254   	] = 0.00010664873843779787421226501464843750000000000000;
	erfc_table_left   [255   	] = 0.00010062195360660552978515625000000000000000000000;
	erfc_table_left   [256   	] = 0.00009491768287261947989463806152343750000000000000;
	erfc_table_left   [257   	] = 0.00008951974450610578060150146484375000000000000000;
	erfc_table_left   [258   	] = 0.00008441272075287997722625732421875000000000000000;
	erfc_table_left   [259   	] = 0.00007958189235068857669830322265625000000000000000;
	erfc_table_left   [260   	] = 0.00007501323852920904755592346191406250000000000000;
	erfc_table_left   [261   	] = 0.00007069339335430413484573364257812500000000000000;
	erfc_table_left   [262   	] = 0.00006660961662419140338897705078125000000000000000;
	erfc_table_left   [263   	] = 0.00006274970655795186758041381835937500000000000000;
	erfc_table_left   [264   	] = 0.00005910227628191933035850524902343750000000000000;
	erfc_table_left   [265   	] = 0.00005565624815062619745731353759765625000000000000;
	erfc_table_left   [266   	] = 0.00005240114478510804474353790283203125000000000000;
	erfc_table_left   [267   	] = 0.00004932700903736986219882965087890625000000000000;
	erfc_table_left   [268   	] = 0.00004642435305868275463581085205078125000000000000;
	erfc_table_left   [269   	] = 0.00004368416557554155588150024414062500000000000000;
	erfc_table_left   [270   	] = 0.00004109786459594033658504486083984375000000000000;
	erfc_table_left   [271   	] = 0.00003865730468533001840114593505859375000000000000;
	erfc_table_left   [272   	] = 0.00003635472603491507470607757568359375000000000000;
	erfc_table_left   [273   	] = 0.00003418276173761114478111267089843750000000000000;
	erfc_table_left   [274   	] = 0.00003213441959815099835395812988281250000000000000;
	erfc_table_left   [275   	] = 0.00003020304211531765758991241455078125000000000000;
	erfc_table_left   [276   	] = 0.00002838231921487022191286087036132812500000000000;
	erfc_table_left   [277   	] = 0.00002666625186975579708814620971679687500000000000;
	erfc_table_left   [278   	] = 0.00002504915028112009167671203613281250000000000000;
	erfc_table_left   [279   	] = 0.00002352560841245576739311218261718750000000000000;
	erfc_table_left   [280   	] = 0.00002209050398960243910551071166992187500000000000;
	erfc_table_left   [281   	] = 0.00002073897121590562164783477783203125000000000000;
	erfc_table_left   [282   	] = 0.00001946639895322732627391815185546875000000000000;
	erfc_table_left   [283   	] = 0.00001826841435104142874479293823242187500000000000;
	erfc_table_left   [284   	] = 0.00001714086829451844096183776855468750000000000000;
	erfc_table_left   [285   	] = 0.00001607983540452551096677780151367187500000000000;
	erfc_table_left   [286   	] = 0.00001508158948126947507262229919433593750000000000;
	erfc_table_left   [287   	] = 0.00001414260350429685786366462707519531250000000000;
	erfc_table_left   [288   	] = 0.00001325951507169520482420921325683593750000000000;
	erfc_table_left   [289   	] = 0.00001242920279764803126454353332519531250000000000;
	erfc_table_left   [290   	] = 0.00001164864988822955638170242309570312500000000000;
	erfc_table_left   [291   	] = 0.00001091502053895965218544006347656250000000000000;
	erfc_table_left   [292   	] = 0.00001022563264996279031038284301757812500000000000;
	erfc_table_left   [293   	] = 0.00000957794691203162074089050292968750000000000000;
	erfc_table_left   [294   	] = 0.00000896956225915346294641494750976562500000000000;
	erfc_table_left   [295   	] = 0.00000839820859255269169807434082031250000000000000;
	erfc_table_left   [296   	] = 0.00000786173950473312288522720336914062500000000000;
	erfc_table_left   [297   	] = 0.00000735812545826775021851062774658203125000000000;
	erfc_table_left   [298   	] = 0.00000688544878357788547873497009277343750000000000;
	erfc_table_left   [299   	] = 0.00000644189776721759699285030364990234375000000000;
	
	erfc_table_right   [0  	 	] = 0.77729743719100952148437500000000000000000000000000;
	erfc_table_right   [1  	 	] = 0.76647806167602539062500000000000000000000000000000;
	erfc_table_right   [2  	 	] = 0.75570410490036010742187500000000000000000000000000;
	erfc_table_right   [3  	 	] = 0.74497741460800170898437500000000000000000000000000;
	erfc_table_right   [4  	 	] = 0.73429995775222778320312500000000000000000000000000;
	erfc_table_right   [5  	 	] = 0.72367358207702636718750000000000000000000000000000;
	erfc_table_right   [6  	 	] = 0.71310025453567504882812500000000000000000000000000;
	erfc_table_right   [7  	 	] = 0.70258176326751708984375000000000000000000000000000;
	erfc_table_right   [8  	 	] = 0.69211995601654052734375000000000000000000000000000;
	erfc_table_right   [9  	 	] = 0.68171650171279907226562500000000000000000000000000;
	erfc_table_right   [10   	] = 0.67137324810028076171875000000000000000000000000000;
	erfc_table_right   [11   	] = 0.66109186410903930664062500000000000000000000000000;
	erfc_table_right   [12   	] = 0.65087401866912841796875000000000000000000000000000;
	erfc_table_right   [13   	] = 0.64072132110595703125000000000000000000000000000000;
	erfc_table_right   [14   	] = 0.63063544034957885742187500000000000000000000000000;
	erfc_table_right   [15   	] = 0.62061792612075805664062500000000000000000000000000;
	erfc_table_right   [16   	] = 0.61067032814025878906250000000000000000000000000000;
	erfc_table_right   [17   	] = 0.60079401731491088867187500000000000000000000000000;
	erfc_table_right   [18   	] = 0.59099054336547851562500000000000000000000000000000;
	erfc_table_right   [19   	] = 0.58126127719879150390625000000000000000000000000000;
	erfc_table_right   [20   	] = 0.57160764932632446289062500000000000000000000000000;
	erfc_table_right   [21   	] = 0.56203091144561767578125000000000000000000000000000;
	erfc_table_right   [22   	] = 0.55253237485885620117187500000000000000000000000000;
	erfc_table_right   [23   	] = 0.54311329126358032226562500000000000000000000000000;
	erfc_table_right   [24   	] = 0.53377491235733032226562500000000000000000000000000;
	erfc_table_right   [25   	] = 0.52451825141906738281250000000000000000000000000000;
	erfc_table_right   [26   	] = 0.51534461975097656250000000000000000000000000000000;
	erfc_table_right   [27   	] = 0.50625491142272949218750000000000000000000000000000;
	erfc_table_right   [28   	] = 0.49725031852722167968750000000000000000000000000000;
	erfc_table_right   [29   	] = 0.48833173513412475585937500000000000000000000000000;
	erfc_table_right   [30   	] = 0.47950011491775512695312500000000000000000000000000;
	erfc_table_right   [31   	] = 0.47075638175010681152343750000000000000000000000000;
	erfc_table_right   [32   	] = 0.46210137009620666503906250000000000000000000000000;
	erfc_table_right   [33   	] = 0.45353588461875915527343750000000000000000000000000;
	erfc_table_right   [34   	] = 0.44506075978279113769531250000000000000000000000000;
	erfc_table_right   [35   	] = 0.43667665123939514160156250000000000000000000000000;
	erfc_table_right   [36   	] = 0.42838421463966369628906250000000000000000000000000;
	erfc_table_right   [37   	] = 0.42018419504165649414062500000000000000000000000000;
	erfc_table_right   [38   	] = 0.41207709908485412597656250000000000000000000000000;
	erfc_table_right   [39   	] = 0.40406352281570434570312500000000000000000000000000;
	erfc_table_right   [40   	] = 0.39614391326904296875000000000000000000000000000000;
	erfc_table_right   [41   	] = 0.38831877708435058593750000000000000000000000000000;
	erfc_table_right   [42   	] = 0.38058856129646301269531250000000000000000000000000;
	erfc_table_right   [43   	] = 0.37295356392860412597656250000000000000000000000000;
	erfc_table_right   [44   	] = 0.36541417241096496582031250000000000000000000000000;
	erfc_table_right   [45   	] = 0.35797068476676940917968750000000000000000000000000;
	erfc_table_right   [46   	] = 0.35062330961227416992187500000000000000000000000000;
	erfc_table_right   [47   	] = 0.34337228536605834960937500000000000000000000000000;
	erfc_table_right   [48   	] = 0.33621779084205627441406250000000000000000000000000;
	erfc_table_right   [49   	] = 0.32915994524955749511718750000000000000000000000000;
	erfc_table_right   [50   	] = 0.32219880819320678710937500000000000000000000000000;
	erfc_table_right   [51   	] = 0.31533446907997131347656250000000000000000000000000;
	erfc_table_right   [52   	] = 0.30856689810752868652343750000000000000000000000000;
	erfc_table_right   [53   	] = 0.30189606547355651855468750000000000000000000000000;
	erfc_table_right   [54   	] = 0.29532191157341003417968750000000000000000000000000;
	erfc_table_right   [55   	] = 0.28884434700012207031250000000000000000000000000000;
	erfc_table_right   [56   	] = 0.28246325254440307617187500000000000000000000000000;
	erfc_table_right   [57   	] = 0.27617838978767395019531250000000000000000000000000;
	erfc_table_right   [58   	] = 0.26998957991600036621093750000000000000000000000000;
	erfc_table_right   [59   	] = 0.26389655470848083496093750000000000000000000000000;
	erfc_table_right   [60   	] = 0.25789901614189147949218750000000000000000000000000;
	erfc_table_right   [61   	] = 0.25199672579765319824218750000000000000000000000000;
	erfc_table_right   [62   	] = 0.24618925154209136962890625000000000000000000000000;
	erfc_table_right   [63   	] = 0.24047625064849853515625000000000000000000000000000;
	erfc_table_right   [64   	] = 0.23485729098320007324218750000000000000000000000000;
	erfc_table_right   [65   	] = 0.22933195531368255615234375000000000000000000000000;
	erfc_table_right   [66   	] = 0.22389972209930419921875000000000000000000000000000;
	erfc_table_right   [67   	] = 0.21856014430522918701171875000000000000000000000000;
	erfc_table_right   [68   	] = 0.21331267058849334716796875000000000000000000000000;
	erfc_table_right   [69   	] = 0.20815674960613250732421875000000000000000000000000;
	erfc_table_right   [70   	] = 0.20309180021286010742187500000000000000000000000000;
	erfc_table_right   [71   	] = 0.19811718165874481201171875000000000000000000000000;
	erfc_table_right   [72   	] = 0.19323226809501647949218750000000000000000000000000;
	erfc_table_right   [73   	] = 0.18843643367290496826171875000000000000000000000000;
	erfc_table_right   [74   	] = 0.18372897803783416748046875000000000000000000000000;
	erfc_table_right   [75   	] = 0.17910918593406677246093750000000000000000000000000;
	erfc_table_right   [76   	] = 0.17457635700702667236328125000000000000000000000000;
	erfc_table_right   [77   	] = 0.17012971639633178710937500000000000000000000000000;
	erfc_table_right   [78   	] = 0.16576850414276123046875000000000000000000000000000;
	erfc_table_right   [79   	] = 0.16149191558361053466796875000000000000000000000000;
	erfc_table_right   [80   	] = 0.15729920566082000732421875000000000000000000000000;
	erfc_table_right   [81   	] = 0.15318951010704040527343750000000000000000000000000;
	erfc_table_right   [82   	] = 0.14916197955608367919921875000000000000000000000000;
	erfc_table_right   [83   	] = 0.14521579444408416748046875000000000000000000000000;
	erfc_table_right   [84   	] = 0.14135006070137023925781250000000000000000000000000;
	erfc_table_right   [85   	] = 0.13756388425827026367187500000000000000000000000000;
	erfc_table_right   [86   	] = 0.13385640084743499755859375000000000000000000000000;
	erfc_table_right   [87   	] = 0.13022670149803161621093750000000000000000000000000;
	erfc_table_right   [88   	] = 0.12667384743690490722656250000000000000000000000000;
	erfc_table_right   [89   	] = 0.12319689989089965820312500000000000000000000000000;
	erfc_table_right   [90   	] = 0.11979493498802185058593750000000000000000000000000;
	erfc_table_right   [91   	] = 0.11646697670221328735351562500000000000000000000000;
	erfc_table_right   [92   	] = 0.11321210116147994995117187500000000000000000000000;
	erfc_table_right   [93   	] = 0.11002932488918304443359375000000000000000000000000;
	erfc_table_right   [94   	] = 0.10691767185926437377929687500000000000000000000000;
	erfc_table_right   [95   	] = 0.10387615859508514404296875000000000000000000000000;
	erfc_table_right   [96   	] = 0.10090380162000656127929687500000000000000000000000;
	erfc_table_right   [97   	] = 0.09799959510564804077148437500000000000000000000000;
	erfc_table_right   [98   	] = 0.09516257047653198242187500000000000000000000000000;
	erfc_table_right   [99   	] = 0.09239171445369720458984375000000000000000000000000;
	erfc_table_right   [100   	] = 0.08968602120876312255859375000000000000000000000000;
	erfc_table_right   [101   	] = 0.08704449236392974853515625000000000000000000000000;
	erfc_table_right   [102   	] = 0.08446612209081649780273437500000000000000000000000;
	erfc_table_right   [103   	] = 0.08194990456104278564453125000000000000000000000000;
	erfc_table_right   [104   	] = 0.07949482649564743041992187500000000000000000000000;
	erfc_table_right   [105   	] = 0.07709988206624984741210937500000000000000000000000;
	erfc_table_right   [106   	] = 0.07476407289505004882812500000000000000000000000000;
	erfc_table_right   [107   	] = 0.07248635590076446533203125000000000000000000000000;
	erfc_table_right   [108   	] = 0.07026579976081848144531250000000000000000000000000;
	erfc_table_right   [109   	] = 0.06810136139392852783203125000000000000000000000000;
	erfc_table_right   [110   	] = 0.06599204987287521362304687500000000000000000000000;
	erfc_table_right   [111   	] = 0.06393687427043914794921875000000000000000000000000;
	erfc_table_right   [112   	] = 0.06193484365940093994140625000000000000000000000000;
	erfc_table_right   [113   	] = 0.05998497456312179565429687500000000000000000000000;
	erfc_table_right   [114   	] = 0.05808628723025321960449218750000000000000000000000;
	erfc_table_right   [115   	] = 0.05623780936002731323242187500000000000000000000000;
	erfc_table_right   [116   	] = 0.05443856865167617797851562500000000000000000000000;
	erfc_table_right   [117   	] = 0.05268760770559310913085937500000000000000000000000;
	erfc_table_right   [118   	] = 0.05098397284746170043945312500000000000000000000000;
	erfc_table_right   [119   	] = 0.04932669550180435180664062500000000000000000000000;
	erfc_table_right   [120   	] = 0.04771487414836883544921875000000000000000000000000;
	erfc_table_right   [121   	] = 0.04614755511283874511718750000000000000000000000000;
	erfc_table_right   [122   	] = 0.04462381824851036071777343750000000000000000000000;
	erfc_table_right   [123   	] = 0.04314274340867996215820312500000000000000000000000;
	erfc_table_right   [124   	] = 0.04170342907309532165527343750000000000000000000000;
	erfc_table_right   [125   	] = 0.04030497372150421142578125000000000000000000000000;
	erfc_table_right   [126   	] = 0.03894649073481559753417968750000000000000000000000;
	erfc_table_right   [127   	] = 0.03762710094451904296875000000000000000000000000000;
	erfc_table_right   [128   	] = 0.03634593635797500610351562500000000000000000000000;
	erfc_table_right   [129   	] = 0.03510214015841484069824218750000000000000000000000;
	erfc_table_right   [130   	] = 0.03389485925436019897460937500000000000000000000000;
	erfc_table_right   [131   	] = 0.03272325918078422546386718750000000000000000000000;
	erfc_table_right   [132   	] = 0.03158649802207946777343750000000000000000000000000;
	erfc_table_right   [133   	] = 0.03048378601670265197753906250000000000000000000000;
	erfc_table_right   [134   	] = 0.02941430732607841491699218750000000000000000000000;
	erfc_table_right   [135   	] = 0.02837726473808288574218750000000000000000000000000;
	erfc_table_right   [136   	] = 0.02737187594175338745117187500000000000000000000000;
	erfc_table_right   [137   	] = 0.02639737166464328765869140625000000000000000000000;
	erfc_table_right   [138   	] = 0.02545299194753170013427734375000000000000000000000;
	erfc_table_right   [139   	] = 0.02453798614442348480224609375000000000000000000000;
	erfc_table_right   [140   	] = 0.02365161851048469543457031250000000000000000000000;
	erfc_table_right   [141   	] = 0.02279316633939743041992187500000000000000000000000;
	erfc_table_right   [142   	] = 0.02196191437542438507080078125000000000000000000000;
	erfc_table_right   [143   	] = 0.02115716412663459777832031250000000000000000000000;
	erfc_table_right   [144   	] = 0.02037821523845195770263671875000000000000000000000;
	erfc_table_right   [145   	] = 0.01962441205978393554687500000000000000000000000000;
	erfc_table_right   [146   	] = 0.01889507658779621124267578125000000000000000000000;
	erfc_table_right   [147   	] = 0.01818955689668655395507812500000000000000000000000;
	erfc_table_right   [148   	] = 0.01750721223652362823486328125000000000000000000000;
	erfc_table_right   [149   	] = 0.01684741303324699401855468750000000000000000000000;
	erfc_table_right   [150   	] = 0.01620954088866710662841796875000000000000000000000;
	erfc_table_right   [151   	] = 0.01559299323707818984985351562500000000000000000000;
	erfc_table_right   [152   	] = 0.01499717403203248977661132812500000000000000000000;
	erfc_table_right   [153   	] = 0.01442150212824344635009765625000000000000000000000;
	erfc_table_right   [154   	] = 0.01386540662497282028198242187500000000000000000000;
	erfc_table_right   [155   	] = 0.01332833152264356613159179687500000000000000000000;
	erfc_table_right   [156   	] = 0.01280972734093666076660156250000000000000000000000;
	erfc_table_right   [157   	] = 0.01230905484408140182495117187500000000000000000000;
	erfc_table_right   [158   	] = 0.01182580180466175079345703125000000000000000000000;
	erfc_table_right   [159   	] = 0.01135944947600364685058593750000000000000000000000;
	erfc_table_right   [160   	] = 0.01090949773788452148437500000000000000000000000000;
	erfc_table_right   [161   	] = 0.01047545485198497772216796875000000000000000000000;
	erfc_table_right   [162   	] = 0.01005684304982423782348632812500000000000000000000;
	erfc_table_right   [163   	] = 0.00965319480746984481811523437500000000000000000000;
	erfc_table_right   [164   	] = 0.00926405284553766250610351562500000000000000000000;
	erfc_table_right   [165   	] = 0.00888897106051445007324218750000000000000000000000;
	erfc_table_right   [166   	] = 0.00852751266211271286010742187500000000000000000000;
	erfc_table_right   [167   	] = 0.00817925389856100082397460937500000000000000000000;
	erfc_table_right   [168   	] = 0.00784377939999103546142578125000000000000000000000;
	erfc_table_right   [169   	] = 0.00752067985013127326965332031250000000000000000000;
	erfc_table_right   [170   	] = 0.00720956921577453613281250000000000000000000000000;
	erfc_table_right   [171   	] = 0.00691005913540720939636230468750000000000000000000;
	erfc_table_right   [172   	] = 0.00662177428603172302246093750000000000000000000000;
	erfc_table_right   [173   	] = 0.00634434912353754043579101562500000000000000000000;
	erfc_table_right   [174   	] = 0.00607742881402373313903808593750000000000000000000;
	erfc_table_right   [175   	] = 0.00582066643983125686645507812500000000000000000000;
	erfc_table_right   [176   	] = 0.00557372486218810081481933593750000000000000000000;
	erfc_table_right   [177   	] = 0.00533627578988671302795410156250000000000000000000;
	erfc_table_right   [178   	] = 0.00510800024494528770446777343750000000000000000000;
	erfc_table_right   [179   	] = 0.00488858763128519058227539062500000000000000000000;
	erfc_table_right   [180   	] = 0.00467773573473095893859863281250000000000000000000;
	erfc_table_right   [181   	] = 0.00447515165433287620544433593750000000000000000000;
	erfc_table_right   [182   	] = 0.00428054761141538619995117187500000000000000000000;
	erfc_table_right   [183   	] = 0.00409365072846412658691406250000000000000000000000;
	erfc_table_right   [184   	] = 0.00391418999060988426208496093750000000000000000000;
	erfc_table_right   [185   	] = 0.00374190346337854862213134765625000000000000000000;
	erfc_table_right   [186   	] = 0.00357653805986046791076660156250000000000000000000;
	erfc_table_right   [187   	] = 0.00341784697957336902618408203125000000000000000000;
	erfc_table_right   [188   	] = 0.00326559133827686309814453125000000000000000000000;
	erfc_table_right   [189   	] = 0.00311953970231115818023681640625000000000000000000;
	erfc_table_right   [190   	] = 0.00297946692444384098052978515625000000000000000000;
	erfc_table_right   [191   	] = 0.00284515530802309513092041015625000000000000000000;
	erfc_table_right   [192   	] = 0.00271639367565512657165527343750000000000000000000;
	erfc_table_right   [193   	] = 0.00259297736920416355133056640625000000000000000000;
	erfc_table_right   [194   	] = 0.00247470661997795104980468750000000000000000000000;
	erfc_table_right   [195   	] = 0.00236139236949384212493896484375000000000000000000;
	erfc_table_right   [196   	] = 0.00225284742191433906555175781250000000000000000000;
	erfc_table_right   [197   	] = 0.00214889156632125377655029296875000000000000000000;
	erfc_table_right   [198   	] = 0.00204935064539313316345214843750000000000000000000;
	erfc_table_right   [199   	] = 0.00195405655540525913238525390625000000000000000000;
	erfc_table_right   [200   	] = 0.00186284631490707397460937500000000000000000000000;
	erfc_table_right   [201   	] = 0.00177556206472218036651611328125000000000000000000;
	erfc_table_right   [202   	] = 0.00169205176644027233123779296875000000000000000000;
	erfc_table_right   [203   	] = 0.00161216815467923879623413085937500000000000000000;
	erfc_table_right   [204   	] = 0.00153576896991580724716186523437500000000000000000;
	erfc_table_right   [205   	] = 0.00146271695848554372787475585937500000000000000000;
	erfc_table_right   [206   	] = 0.00139287929050624370574951171875000000000000000000;
	erfc_table_right   [207   	] = 0.00132612790912389755249023437500000000000000000000;
	erfc_table_right   [208   	] = 0.00126233929768204689025878906250000000000000000000;
	erfc_table_right   [209   	] = 0.00120139413047581911087036132812500000000000000000;
	erfc_table_right   [210   	] = 0.00114317715633660554885864257812500000000000000000;
	erfc_table_right   [211   	] = 0.00108757743146270513534545898437500000000000000000;
	erfc_table_right   [212   	] = 0.00103448797017335891723632812500000000000000000000;
	erfc_table_right   [213   	] = 0.00098380446434020996093750000000000000000000000000;
	erfc_table_right   [214   	] = 0.00093542964896187186241149902343750000000000000000;
	erfc_table_right   [215   	] = 0.00088926660828292369842529296875000000000000000000;
	erfc_table_right   [216   	] = 0.00084522296674549579620361328125000000000000000000;
	erfc_table_right   [217   	] = 0.00080320989945903420448303222656250000000000000000;
	erfc_table_right   [218   	] = 0.00076314172474667429924011230468750000000000000000;
	erfc_table_right   [219   	] = 0.00072493613697588443756103515625000000000000000000;
	erfc_table_right   [220   	] = 0.00068851374089717864990234375000000000000000000000;
	erfc_table_right   [221   	] = 0.00065379828447476029396057128906250000000000000000;
	erfc_table_right   [222   	] = 0.00062071642605587840080261230468750000000000000000;
	erfc_table_right   [223   	] = 0.00058919755974784493446350097656250000000000000000;
	erfc_table_right   [224   	] = 0.00055917387362569570541381835937500000000000000000;
	erfc_table_right   [225   	] = 0.00053058011690154671669006347656250000000000000000;
	erfc_table_right   [226   	] = 0.00050335365813225507736206054687500000000000000000;
	erfc_table_right   [227   	] = 0.00047743431059643626213073730468750000000000000000;
	erfc_table_right   [228   	] = 0.00045276424498297274112701416015625000000000000000;
	erfc_table_right   [229   	] = 0.00042928796028718352317810058593750000000000000000;
	erfc_table_right   [230   	] = 0.00040695210918784141540527343750000000000000000000;
	erfc_table_right   [231   	] = 0.00038570561446249485015869140625000000000000000000;
	erfc_table_right   [232   	] = 0.00036549931974150240421295166015625000000000000000;
	erfc_table_right   [233   	] = 0.00034628616413101553916931152343750000000000000000;
	erfc_table_right   [234   	] = 0.00032802097848616540431976318359375000000000000000;
	erfc_table_right   [235   	] = 0.00031066051451489329338073730468750000000000000000;
	erfc_table_right   [236   	] = 0.00029416318284347653388977050781250000000000000000;
	erfc_table_right   [237   	] = 0.00027848925674334168434143066406250000000000000000;
	erfc_table_right   [238   	] = 0.00026360026095062494277954101562500000000000000000;
	erfc_table_right   [239   	] = 0.00024946039775386452674865722656250000000000000000;
	erfc_table_right   [240   	] = 0.00023603429144714027643203735351562500000000000000;
	erfc_table_right   [241   	] = 0.00022328850172925740480422973632812500000000000000;
	erfc_table_right   [242   	] = 0.00021119097073096781969070434570312500000000000000;
	erfc_table_right   [243   	] = 0.00019971100846305489540100097656250000000000000000;
	erfc_table_right   [244   	] = 0.00018881927826441824436187744140625000000000000000;
	erfc_table_right   [245   	] = 0.00017848770949058234691619873046875000000000000000;
	erfc_table_right   [246   	] = 0.00016868943930603563785552978515625000000000000000;
	erfc_table_right   [247   	] = 0.00015939879813231527805328369140625000000000000000;
	erfc_table_right   [248   	] = 0.00015059126599226146936416625976562500000000000000;
	erfc_table_right   [249   	] = 0.00014224338519852608442306518554687500000000000000;
	erfc_table_right   [250   	] = 0.00013433274580165743827819824218750000000000000000;
	erfc_table_right   [251   	] = 0.00012683792738243937492370605468750000000000000000;
	erfc_table_right   [252   	] = 0.00011973852815572172403335571289062500000000000000;
	erfc_table_right   [253   	] = 0.00011301500489935278892517089843750000000000000000;
	erfc_table_right   [254   	] = 0.00010664873843779787421226501464843750000000000000;
	erfc_table_right   [255   	] = 0.00010062195360660552978515625000000000000000000000;
	erfc_table_right   [256   	] = 0.00009491768287261947989463806152343750000000000000;
	erfc_table_right   [257   	] = 0.00008951974450610578060150146484375000000000000000;
	erfc_table_right   [258   	] = 0.00008441272075287997722625732421875000000000000000;
	erfc_table_right   [259   	] = 0.00007958189235068857669830322265625000000000000000;
	erfc_table_right   [260   	] = 0.00007501323852920904755592346191406250000000000000;
	erfc_table_right   [261   	] = 0.00007069339335430413484573364257812500000000000000;
	erfc_table_right   [262   	] = 0.00006660961662419140338897705078125000000000000000;
	erfc_table_right   [263   	] = 0.00006274970655795186758041381835937500000000000000;
	erfc_table_right   [264   	] = 0.00005910227628191933035850524902343750000000000000;
	erfc_table_right   [265   	] = 0.00005565624815062619745731353759765625000000000000;
	erfc_table_right   [266   	] = 0.00005240114478510804474353790283203125000000000000;
	erfc_table_right   [267   	] = 0.00004932700903736986219882965087890625000000000000;
	erfc_table_right   [268   	] = 0.00004642435305868275463581085205078125000000000000;
	erfc_table_right   [269   	] = 0.00004368416557554155588150024414062500000000000000;
	erfc_table_right   [270   	] = 0.00004109786459594033658504486083984375000000000000;
	erfc_table_right   [271   	] = 0.00003865730468533001840114593505859375000000000000;
	erfc_table_right   [272   	] = 0.00003635472603491507470607757568359375000000000000;
	erfc_table_right   [273   	] = 0.00003418276173761114478111267089843750000000000000;
	erfc_table_right   [274   	] = 0.00003213441959815099835395812988281250000000000000;
	erfc_table_right   [275   	] = 0.00003020304211531765758991241455078125000000000000;
	erfc_table_right   [276   	] = 0.00002838231921487022191286087036132812500000000000;
	erfc_table_right   [277   	] = 0.00002666625186975579708814620971679687500000000000;
	erfc_table_right   [278   	] = 0.00002504915028112009167671203613281250000000000000;
	erfc_table_right   [279   	] = 0.00002352560841245576739311218261718750000000000000;
	erfc_table_right   [280   	] = 0.00002209050398960243910551071166992187500000000000;
	erfc_table_right   [281   	] = 0.00002073897121590562164783477783203125000000000000;
	erfc_table_right   [282   	] = 0.00001946639895322732627391815185546875000000000000;
	erfc_table_right   [283   	] = 0.00001826841435104142874479293823242187500000000000;
	erfc_table_right   [284   	] = 0.00001714086829451844096183776855468750000000000000;
	erfc_table_right   [285   	] = 0.00001607983540452551096677780151367187500000000000;
	erfc_table_right   [286   	] = 0.00001508158948126947507262229919433593750000000000;
	erfc_table_right   [287   	] = 0.00001414260350429685786366462707519531250000000000;
	erfc_table_right   [288   	] = 0.00001325951507169520482420921325683593750000000000;
	erfc_table_right   [289   	] = 0.00001242920279764803126454353332519531250000000000;
	erfc_table_right   [290   	] = 0.00001164864988822955638170242309570312500000000000;
	erfc_table_right   [291   	] = 0.00001091502053895965218544006347656250000000000000;
	erfc_table_right   [292   	] = 0.00001022563264996279031038284301757812500000000000;
	erfc_table_right   [293   	] = 0.00000957794691203162074089050292968750000000000000;
	erfc_table_right   [294   	] = 0.00000896956225915346294641494750976562500000000000;
	erfc_table_right   [295   	] = 0.00000839820859255269169807434082031250000000000000;
	erfc_table_right   [296   	] = 0.00000786173950473312288522720336914062500000000000;
	erfc_table_right   [297   	] = 0.00000735812545826775021851062774658203125000000000;
	erfc_table_right   [298   	] = 0.00000688544878357788547873497009277343750000000000;
	erfc_table_right   [299   	] = 0.00000644189776721759699285030364990234375000000000;


        int left_inx = (int)((input - 0.2f) * 100.0f);
        int right_inx = left_inx + 1;

	float left_temp = erfc_table_left[left_inx];
        return (left_temp + 100.00f * (erfc_table_right[right_inx] - left_temp) * (float)(input - (0.2f + (float) left_inx / 100.00f)));

}


//	implementation of my_exp_table
#define STEPS 	1000
float my_exp (float input)
{
    float exp_table [STEPS];

    exp_table [0	] = 1.00000000000000000000000000000000000000000000000000f;
    exp_table [1	] = 0.99004983901977539062500000000000000000000000000000f;
    exp_table [2	] = 0.98019868135452270507812500000000000000000000000000f;
    exp_table [3	] = 0.97044551372528076171875000000000000000000000000000f;
    exp_table [4	] = 0.96078944206237792968750000000000000000000000000000f;
    exp_table [5	] = 0.95122945308685302734375000000000000000000000000000f;
    exp_table [6	] = 0.94176453351974487304687500000000000000000000000000f;
    exp_table [7	] = 0.93239384889602661132812500000000000000000000000000f;
    exp_table [8	] = 0.92311632633209228515625000000000000000000000000000f;
    exp_table [9	] = 0.91393119096755981445312500000000000000000000000000f;
    exp_table [10	] = 0.90483742952346801757812500000000000000000000000000f;
    exp_table [11	] = 0.89583414793014526367187500000000000000000000000000f;
    exp_table [12	] = 0.88692045211791992187500000000000000000000000000000f;
    exp_table [13	] = 0.87809544801712036132812500000000000000000000000000f;
    exp_table [14	] = 0.86935824155807495117187500000000000000000000000000f;
    exp_table [15	] = 0.86070799827575683593750000000000000000000000000000f;
    exp_table [16	] = 0.85214376449584960937500000000000000000000000000000f;
    exp_table [17	] = 0.84366482496261596679687500000000000000000000000000f;
    exp_table [18	] = 0.83527022600173950195312500000000000000000000000000f;
    exp_table [19	] = 0.82695913314819335937500000000000000000000000000000f;
    exp_table [20	] = 0.81873077154159545898437500000000000000000000000000f;
    exp_table [21	] = 0.81058424711227416992187500000000000000000000000000f;
    exp_table [22	] = 0.80251878499984741210937500000000000000000000000000f;
    exp_table [23	] = 0.79453361034393310546875000000000000000000000000000f;
    exp_table [24	] = 0.78662788867950439453125000000000000000000000000000f;
    exp_table [25	] = 0.77880078554153442382812500000000000000000000000000f;
    exp_table [26	] = 0.77105158567428588867187500000000000000000000000000f;
    exp_table [27	] = 0.76337951421737670898437500000000000000000000000000f;
    exp_table [28	] = 0.75578373670578002929687500000000000000000000000000f;
    exp_table [29	] = 0.74826359748840332031250000000000000000000000000000f;
    exp_table [30	] = 0.74081820249557495117187500000000000000000000000000f;
    exp_table [31	] = 0.73344695568084716796875000000000000000000000000000f;
    exp_table [32	] = 0.72614902257919311523437500000000000000000000000000f;
    exp_table [33	] = 0.71892374753952026367187500000000000000000000000000f;
    exp_table [34	] = 0.71177029609680175781250000000000000000000000000000f;
    exp_table [35	] = 0.70468807220458984375000000000000000000000000000000f;
    exp_table [36	] = 0.69767630100250244140625000000000000000000000000000f;
    exp_table [37	] = 0.69073432683944702148437500000000000000000000000000f;
    exp_table [38	] = 0.68386143445968627929687500000000000000000000000000f;
    exp_table [39	] = 0.67705690860748291015625000000000000000000000000000f;
    exp_table [40	] = 0.67032003402709960937500000000000000000000000000000f;
    exp_table [41	] = 0.66365027427673339843750000000000000000000000000000f;
    exp_table [42	] = 0.65704685449600219726562500000000000000000000000000f;
    exp_table [43	] = 0.65050911903381347656250000000000000000000000000000f;
    exp_table [44	] = 0.64403641223907470703125000000000000000000000000000f;
    exp_table [45	] = 0.63762813806533813476562500000000000000000000000000f;
    exp_table [46	] = 0.63128364086151123046875000000000000000000000000000f;
    exp_table [47	] = 0.62500226497650146484375000000000000000000000000000f;
    exp_table [48	] = 0.61878341436386108398437500000000000000000000000000f;
    exp_table [49	] = 0.61262637376785278320312500000000000000000000000000f;
    exp_table [50	] = 0.60653066635131835937500000000000000000000000000000f;
    exp_table [51	] = 0.60049557685852050781250000000000000000000000000000f;
    exp_table [52	] = 0.59452056884765625000000000000000000000000000000000f;
    exp_table [53	] = 0.58860498666763305664062500000000000000000000000000f;
    exp_table [54	] = 0.58274823427200317382812500000000000000000000000000f;
    exp_table [55	] = 0.57694977521896362304687500000000000000000000000000f;
    exp_table [56	] = 0.57120907306671142578125000000000000000000000000000f;
    exp_table [57	] = 0.56552547216415405273437500000000000000000000000000f;
    exp_table [58	] = 0.55989837646484375000000000000000000000000000000000f;
    exp_table [59	] = 0.55432730913162231445312500000000000000000000000000f;
    exp_table [60	] = 0.54881161451339721679687500000000000000000000000000f;
    exp_table [61	] = 0.54335087537765502929687500000000000000000000000000f;
    exp_table [62	] = 0.53794443607330322265625000000000000000000000000000f;
    exp_table [63	] = 0.53259181976318359375000000000000000000000000000000f;
    exp_table [64	] = 0.52729243040084838867187500000000000000000000000000f;
    exp_table [65	] = 0.52204579114913940429687500000000000000000000000000f;
    exp_table [66	] = 0.51685130596160888671875000000000000000000000000000f;
    exp_table [67	] = 0.51170855760574340820312500000000000000000000000000f;
    exp_table [68	] = 0.50661700963973999023437500000000000000000000000000f;
    exp_table [69	] = 0.50157606601715087890625000000000000000000000000000f;
    exp_table [70	] = 0.49658530950546264648437500000000000000000000000000f;
    exp_table [71	] = 0.49164420366287231445312500000000000000000000000000f;
    exp_table [72	] = 0.48675224184989929199218750000000000000000000000000f;
    exp_table [73	] = 0.48190897703170776367187500000000000000000000000000f;
    exp_table [74	] = 0.47711390256881713867187500000000000000000000000000f;
    exp_table [75	] = 0.47236654162406921386718750000000000000000000000000f;
    exp_table [76	] = 0.46766641736030578613281250000000000000000000000000f;
    exp_table [77	] = 0.46301308274269104003906250000000000000000000000000f;
    exp_table [78	] = 0.45840603113174438476562500000000000000000000000000f;
    exp_table [79	] = 0.45384478569030761718750000000000000000000000000000f;
    exp_table [80	] = 0.44932895898818969726562500000000000000000000000000f;
    exp_table [81	] = 0.44485807418823242187500000000000000000000000000000f;
    exp_table [82	] = 0.44043165445327758789062500000000000000000000000000f;
    exp_table [83	] = 0.43604928255081176757812500000000000000000000000000f;
    exp_table [84	] = 0.43171054124832153320312500000000000000000000000000f;
    exp_table [85	] = 0.42741492390632629394531250000000000000000000000000f;
    exp_table [86	] = 0.42316207289695739746093750000000000000000000000000f;
    exp_table [87	] = 0.41895154118537902832031250000000000000000000000000f;
    exp_table [88	] = 0.41478291153907775878906250000000000000000000000000f;
    exp_table [89	] = 0.41065576672554016113281250000000000000000000000000f;
    exp_table [90	] = 0.40656965970993041992187500000000000000000000000000f;
    exp_table [91	] = 0.40252420306205749511718750000000000000000000000000f;
    exp_table [92	] = 0.39851903915405273437500000000000000000000000000000f;
    exp_table [93	] = 0.39455372095108032226562500000000000000000000000000f;
    exp_table [94	] = 0.39062783122062683105468750000000000000000000000000f;
    exp_table [95	] = 0.38674104213714599609375000000000000000000000000000f;
    exp_table [96	] = 0.38289290666580200195312500000000000000000000000000f;
    exp_table [97	] = 0.37908303737640380859375000000000000000000000000000f;
    exp_table [98	] = 0.37531110644340515136718750000000000000000000000000f;
    exp_table [99	] = 0.37157669663429260253906250000000000000000000000000f;
    exp_table [100	] = 0.36787945032119750976562500000000000000000000000000f;
    exp_table [101	] = 0.36421898007392883300781250000000000000000000000000f;
    exp_table [102	] = 0.36059495806694030761718750000000000000000000000000f;
    exp_table [103	] = 0.35700696706771850585937500000000000000000000000000f;
    exp_table [104	] = 0.35345470905303955078125000000000000000000000000000f;
    exp_table [105	] = 0.34993776679039001464843750000000000000000000000000f;
    exp_table [106	] = 0.34645584225654602050781250000000000000000000000000f;
    exp_table [107	] = 0.34300848841667175292968750000000000000000000000000f;
    exp_table [108	] = 0.33959549665451049804687500000000000000000000000000f;
    exp_table [109	] = 0.33621647953987121582031250000000000000000000000000f;
    exp_table [110	] = 0.33287107944488525390625000000000000000000000000000f;
    exp_table [111	] = 0.32955896854400634765625000000000000000000000000000f;
    exp_table [112	] = 0.32627978920936584472656250000000000000000000000000f;
    exp_table [113	] = 0.32303324341773986816406250000000000000000000000000f;
    exp_table [114	] = 0.31981903314590454101562500000000000000000000000000f;
    exp_table [115	] = 0.31663677096366882324218750000000000000000000000000f;
    exp_table [116	] = 0.31348618865013122558593750000000000000000000000000f;
    exp_table [117	] = 0.31036695837974548339843750000000000000000000000000f;
    exp_table [118	] = 0.30727875232696533203125000000000000000000000000000f;
    exp_table [119	] = 0.30422124266624450683593750000000000000000000000000f;
    exp_table [120	] = 0.30119419097900390625000000000000000000000000000000f;
    exp_table [121	] = 0.29819726943969726562500000000000000000000000000000f;
    exp_table [122	] = 0.29523015022277832031250000000000000000000000000000f;
    exp_table [123	] = 0.29229256510734558105468750000000000000000000000000f;
    exp_table [124	] = 0.28938421607017517089843750000000000000000000000000f;
    exp_table [125	] = 0.28650480508804321289062500000000000000000000000000f;
    exp_table [126	] = 0.28365403413772583007812500000000000000000000000000f;
    exp_table [127	] = 0.28083163499832153320312500000000000000000000000000f;
    exp_table [128	] = 0.27803730964660644531250000000000000000000000000000f;
    exp_table [129	] = 0.27527078986167907714843750000000000000000000000000f;
    exp_table [130	] = 0.27253180742263793945312500000000000000000000000000f;
    exp_table [131	] = 0.26982006430625915527343750000000000000000000000000f;
    exp_table [132	] = 0.26713529229164123535156250000000000000000000000000f;
    exp_table [133	] = 0.26447725296020507812500000000000000000000000000000f;
    exp_table [134	] = 0.26184564828872680664062500000000000000000000000000f;
    exp_table [135	] = 0.25924023985862731933593750000000000000000000000000f;
    exp_table [136	] = 0.25666075944900512695312500000000000000000000000000f;
    exp_table [137	] = 0.25410696864128112792968750000000000000000000000000f;
    exp_table [138	] = 0.25157853960990905761718750000000000000000000000000f;
    exp_table [139	] = 0.24907530844211578369140625000000000000000000000000f;
    exp_table [140	] = 0.24659697711467742919921875000000000000000000000000f;
    exp_table [141	] = 0.24414329230785369873046875000000000000000000000000f;
    exp_table [142	] = 0.24171403050422668457031250000000000000000000000000f;
    exp_table [143	] = 0.23930893838405609130859375000000000000000000000000f;
    exp_table [144	] = 0.23692774772644042968750000000000000000000000000000f;
    exp_table [145	] = 0.23457027971744537353515625000000000000000000000000f;
    exp_table [146	] = 0.23223626613616943359375000000000000000000000000000f;
    exp_table [147	] = 0.22992548346519470214843750000000000000000000000000f;
    exp_table [148	] = 0.22763767838478088378906250000000000000000000000000f;
    exp_table [149	] = 0.22537265717983245849609375000000000000000000000000f;
    exp_table [150	] = 0.22313016653060913085937500000000000000000000000000f;
    exp_table [151	] = 0.22090998291969299316406250000000000000000000000000f;
    exp_table [152	] = 0.21871189773082733154296875000000000000000000000000f;
    exp_table [153	] = 0.21653567254543304443359375000000000000000000000000f;
    exp_table [154	] = 0.21438111364841461181640625000000000000000000000000f;
    exp_table [155	] = 0.21224798262119293212890625000000000000000000000000f;
    exp_table [156	] = 0.21013608574867248535156250000000000000000000000000f;
    exp_table [157	] = 0.20804516971111297607421875000000000000000000000000f;
    exp_table [158	] = 0.20597508549690246582031250000000000000000000000000f;
    exp_table [159	] = 0.20392560958862304687500000000000000000000000000000f;
    exp_table [160	] = 0.20189651846885681152343750000000000000000000000000f;
    exp_table [161	] = 0.19988761842250823974609375000000000000000000000000f;
    exp_table [162	] = 0.19789870083332061767578125000000000000000000000000f;
    exp_table [163	] = 0.19592957198619842529296875000000000000000000000000f;
    exp_table [164	] = 0.19398003816604614257812500000000000000000000000000f;
    exp_table [165	] = 0.19204992055892944335937500000000000000000000000000f;
    exp_table [166	] = 0.19013898074626922607421875000000000000000000000000f;
    exp_table [167	] = 0.18824706971645355224609375000000000000000000000000f;
    exp_table [168	] = 0.18637397885322570800781250000000000000000000000000f;
    exp_table [169	] = 0.18451951444149017333984375000000000000000000000000f;
    exp_table [170	] = 0.18268351256847381591796875000000000000000000000000f;
    exp_table [171	] = 0.18086577951908111572265625000000000000000000000000f;
    exp_table [172	] = 0.17906613647937774658203125000000000000000000000000f;
    exp_table [173	] = 0.17728440463542938232421875000000000000000000000000f;
    exp_table [174	] = 0.17552040517330169677734375000000000000000000000000f;
    exp_table [175	] = 0.17377394437789916992187500000000000000000000000000f;
    exp_table [176	] = 0.17204485833644866943359375000000000000000000000000f;
    exp_table [177	] = 0.17033299803733825683593750000000000000000000000000f;
    exp_table [178	] = 0.16863815486431121826171875000000000000000000000000f;
    exp_table [179	] = 0.16696017980575561523437500000000000000000000000000f;
    exp_table [180	] = 0.16529889404773712158203125000000000000000000000000f;
    exp_table [181	] = 0.16365414857864379882812500000000000000000000000000f;
    exp_table [182	] = 0.16202574968338012695312500000000000000000000000000f;
    exp_table [183	] = 0.16041356325149536132812500000000000000000000000000f;
    exp_table [184	] = 0.15881742537021636962890625000000000000000000000000f;
    exp_table [185	] = 0.15723715722560882568359375000000000000000000000000f;
    exp_table [186	] = 0.15567262470722198486328125000000000000000000000000f;
    exp_table [187	] = 0.15412366390228271484375000000000000000000000000000f;
    exp_table [188	] = 0.15259011089801788330078125000000000000000000000000f;
    exp_table [189	] = 0.15107181668281555175781250000000000000000000000000f;
    exp_table [190	] = 0.14956861734390258789062500000000000000000000000000f;
    exp_table [191	] = 0.14808039367198944091796875000000000000000000000000f;
    exp_table [192	] = 0.14660696685314178466796875000000000000000000000000f;
    exp_table [193	] = 0.14514820277690887451171875000000000000000000000000f;
    exp_table [194	] = 0.14370393753051757812500000000000000000000000000000f;
    exp_table [195	] = 0.14227406680583953857421875000000000000000000000000f;
    exp_table [196	] = 0.14085841178894042968750000000000000000000000000000f;
    exp_table [197	] = 0.13945685327053070068359375000000000000000000000000f;
    exp_table [198	] = 0.13806922733783721923828125000000000000000000000000f;
    exp_table [199	] = 0.13669542968273162841796875000000000000000000000000f;
    exp_table [200	] = 0.13533528149127960205078125000000000000000000000000f;
    exp_table [201	] = 0.13398867845535278320312500000000000000000000000000f;
    exp_table [202	] = 0.13265547156333923339843750000000000000000000000000f;
    exp_table [203	] = 0.13133552670478820800781250000000000000000000000000f;
    exp_table [204	] = 0.13002870976924896240234375000000000000000000000000f;
    exp_table [205	] = 0.12873491644859313964843750000000000000000000000000f;
    exp_table [206	] = 0.12745398283004760742187500000000000000000000000000f;
    exp_table [207	] = 0.12618578970432281494140625000000000000000000000000f;
    exp_table [208	] = 0.12493022531270980834960937500000000000000000000000f;
    exp_table [209	] = 0.12368714809417724609375000000000000000000000000000f;
    exp_table [210	] = 0.12245643883943557739257812500000000000000000000000f;
    exp_table [211	] = 0.12123797833919525146484375000000000000000000000000f;
    exp_table [212	] = 0.12003163993358612060546875000000000000000000000000f;
    exp_table [213	] = 0.11883728206157684326171875000000000000000000000000f;
    exp_table [214	] = 0.11765483021736145019531250000000000000000000000000f;
    exp_table [215	] = 0.11648415029048919677734375000000000000000000000000f;
    exp_table [216	] = 0.11532510817050933837890625000000000000000000000000f;
    exp_table [217	] = 0.11417760699987411499023437500000000000000000000000f;
    exp_table [218	] = 0.11304152011871337890625000000000000000000000000000f;
    exp_table [219	] = 0.11191674321889877319335937500000000000000000000000f;
    exp_table [220	] = 0.11080314964056015014648437500000000000000000000000f;
    exp_table [221	] = 0.10970064252614974975585937500000000000000000000000f;
    exp_table [222	] = 0.10860910266637802124023437500000000000000000000000f;
    exp_table [223	] = 0.10752842575311660766601562500000000000000000000000f;
    exp_table [224	] = 0.10645850002765655517578125000000000000000000000000f;
    exp_table [225	] = 0.10539922118186950683593750000000000000000000000000f;
    exp_table [226	] = 0.10435048490762710571289062500000000000000000000000f;
    exp_table [227	] = 0.10331217944622039794921875000000000000000000000000f;
    exp_table [228	] = 0.10228420794010162353515625000000000000000000000000f;
    exp_table [229	] = 0.10126646608114242553710937500000000000000000000000f;
    exp_table [230	] = 0.10025884956121444702148437500000000000000000000000f;
    exp_table [231	] = 0.09926125407218933105468750000000000000000000000000f;
    exp_table [232	] = 0.09827359020709991455078125000000000000000000000000f;
    exp_table [233	] = 0.09729575365781784057617187500000000000000000000000f;
    exp_table [234	] = 0.09632764756679534912109375000000000000000000000000f;
    exp_table [235	] = 0.09536916762590408325195312500000000000000000000000f;
    exp_table [236	] = 0.09442023187875747680664062500000000000000000000000f;
    exp_table [237	] = 0.09348073601722717285156250000000000000000000000000f;
    exp_table [238	] = 0.09255056828260421752929687500000000000000000000000f;
    exp_table [239	] = 0.09162967652082443237304687500000000000000000000000f;
    exp_table [240	] = 0.09071794152259826660156250000000000000000000000000f;
    exp_table [241	] = 0.08981528878211975097656250000000000000000000000000f;
    exp_table [242	] = 0.08892161399126052856445312500000000000000000000000f;
    exp_table [243	] = 0.08803682774305343627929687500000000000000000000000f;
    exp_table [244	] = 0.08716084808111190795898437500000000000000000000000f;
    exp_table [245	] = 0.08629358559846878051757812500000000000000000000000f;
    exp_table [246	] = 0.08543495088815689086914062500000000000000000000000f;
    exp_table [247	] = 0.08458485454320907592773437500000000000000000000000f;
    exp_table [248	] = 0.08374322205781936645507812500000000000000000000000f;
    exp_table [249	] = 0.08290996402502059936523437500000000000000000000000f;
    exp_table [250	] = 0.08208499848842620849609375000000000000000000000000f;
    exp_table [251	] = 0.08126824349164962768554687500000000000000000000000f;
    exp_table [252	] = 0.08045960962772369384765625000000000000000000000000f;
    exp_table [253	] = 0.07965902239084243774414062500000000000000000000000f;
    exp_table [254	] = 0.07886639982461929321289062500000000000000000000000f;
    exp_table [255	] = 0.07808166742324829101562500000000000000000000000000f;
    exp_table [256	] = 0.07730474323034286499023437500000000000000000000000f;
    exp_table [257	] = 0.07653555274009704589843750000000000000000000000000f;
    exp_table [258	] = 0.07577400654554367065429687500000000000000000000000f;
    exp_table [259	] = 0.07502004504203796386718750000000000000000000000000f;
    exp_table [260	] = 0.07427358627319335937500000000000000000000000000000f;
    exp_table [261	] = 0.07353454828262329101562500000000000000000000000000f;
    exp_table [262	] = 0.07280287146568298339843750000000000000000000000000f;
    exp_table [263	] = 0.07207845151424407958984375000000000000000000000000f;
    exp_table [264	] = 0.07136125862598419189453125000000000000000000000000f;
    exp_table [265	] = 0.07065120339393615722656250000000000000000000000000f;
    exp_table [266	] = 0.06994821876287460327148437500000000000000000000000f;
    exp_table [267	] = 0.06925222277641296386718750000000000000000000000000f;
    exp_table [268	] = 0.06856314837932586669921875000000000000000000000000f;
    exp_table [269	] = 0.06788093596696853637695312500000000000000000000000f;
    exp_table [270	] = 0.06720551103353500366210937500000000000000000000000f;
    exp_table [271	] = 0.06653680652379989624023437500000000000000000000000f;
    exp_table [272	] = 0.06587475538253784179687500000000000000000000000000f;
    exp_table [273	] = 0.06521929055452346801757812500000000000000000000000f;
    exp_table [274	] = 0.06457034498453140258789062500000000000000000000000f;
    exp_table [275	] = 0.06392785906791687011718750000000000000000000000000f;
    exp_table [276	] = 0.06329176574945449829101562500000000000000000000000f;
    exp_table [277	] = 0.06266200542449951171875000000000000000000000000000f;
    exp_table [278	] = 0.06203850731253623962402343750000000000000000000000f;
    exp_table [279	] = 0.06142121553421020507812500000000000000000000000000f;
    exp_table [280	] = 0.06081006675958633422851562500000000000000000000000f;
    exp_table [281	] = 0.06020499765872955322265625000000000000000000000000f;
    exp_table [282	] = 0.05960594490170478820800781250000000000000000000000f;
    exp_table [283	] = 0.05901285633444786071777343750000000000000000000000f;
    exp_table [284	] = 0.05842567235231399536132812500000000000000000000000f;
    exp_table [285	] = 0.05784432590007781982421875000000000000000000000000f;
    exp_table [286	] = 0.05726876482367515563964843750000000000000000000000f;
    exp_table [287	] = 0.05669893324375152587890625000000000000000000000000f;
    exp_table [288	] = 0.05613475665450096130371093750000000000000000000000f;
    exp_table [289	] = 0.05557620525360107421875000000000000000000000000000f;
    exp_table [290	] = 0.05502321571111679077148437500000000000000000000000f;
    exp_table [291	] = 0.05447572469711303710937500000000000000000000000000f;
    exp_table [292	] = 0.05393368378281593322753906250000000000000000000000f;
    exp_table [293	] = 0.05339703336358070373535156250000000000000000000000f;
    exp_table [294	] = 0.05286572501063346862792968750000000000000000000000f;
    exp_table [295	] = 0.05233970284461975097656250000000000000000000000000f;
    exp_table [296	] = 0.05181891471147537231445312500000000000000000000000f;
    exp_table [297	] = 0.05130330845713615417480468750000000000000000000000f;
    exp_table [298	] = 0.05079283192753791809082031250000000000000000000000f;
    exp_table [299	] = 0.05028743669390678405761718750000000000000000000000f;
    exp_table [300	] = 0.04978706687688827514648437500000000000000000000000f;
    exp_table [301	] = 0.04929167777299880981445312500000000000000000000000f;
    exp_table [302	] = 0.04880122095346450805664062500000000000000000000000f;
    exp_table [303	] = 0.04831564053893089294433593750000000000000000000000f;
    exp_table [304	] = 0.04783489182591438293457031250000000000000000000000f;
    exp_table [305	] = 0.04735892638564109802246093750000000000000000000000f;
    exp_table [306	] = 0.04688769951462745666503906250000000000000000000000f;
    exp_table [307	] = 0.04642115905880928039550781250000000000000000000000f;
    exp_table [308	] = 0.04595926031470298767089843750000000000000000000000f;
    exp_table [309	] = 0.04550195857882499694824218750000000000000000000000f;
    exp_table [310	] = 0.04504920542240142822265625000000000000000000000000f;
    exp_table [311	] = 0.04460095986723899841308593750000000000000000000000f;
    exp_table [312	] = 0.04415717348456382751464843750000000000000000000000f;
    exp_table [313	] = 0.04371779039502143859863281250000000000000000000000f;
    exp_table [314	] = 0.04328279197216033935546875000000000000000000000000f;
    exp_table [315	] = 0.04285212233662605285644531250000000000000000000000f;
    exp_table [316	] = 0.04242573678493499755859375000000000000000000000000f;
    exp_table [317	] = 0.04200359433889389038085937500000000000000000000000f;
    exp_table [318	] = 0.04158565402030944824218750000000000000000000000000f;
    exp_table [319	] = 0.04117186740040779113769531250000000000000000000000f;
    exp_table [320	] = 0.04076220095157623291015625000000000000000000000000f;
    exp_table [321	] = 0.04035660997033119201660156250000000000000000000000f;
    exp_table [322	] = 0.03995505720376968383789062500000000000000000000000f;
    exp_table [323	] = 0.03955749794840812683105468750000000000000000000000f;
    exp_table [324	] = 0.03916389495134353637695312500000000000000000000000f;
    exp_table [325	] = 0.03877420723438262939453125000000000000000000000000f;
    exp_table [326	] = 0.03838839754462242126464843750000000000000000000000f;
    exp_table [327	] = 0.03800642862915992736816406250000000000000000000000f;
    exp_table [328	] = 0.03762825950980186462402343750000000000000000000000f;
    exp_table [329	] = 0.03725384920835494995117187500000000000000000000000f;
    exp_table [330	] = 0.03688316792249679565429687500000000000000000000000f;
    exp_table [331	] = 0.03651617467403411865234375000000000000000000000000f;
    exp_table [332	] = 0.03615283593535423278808593750000000000000000000000f;
    exp_table [333	] = 0.03579310700297355651855468750000000000000000000000f;
    exp_table [334	] = 0.03543696179986000061035156250000000000000000000000f;
    exp_table [335	] = 0.03508435562252998352050781250000000000000000000000f;
    exp_table [336	] = 0.03473526239395141601562500000000000000000000000000f;
    exp_table [337	] = 0.03438964113593101501464843750000000000000000000000f;
    exp_table [338	] = 0.03404745087027549743652343750000000000000000000000f;
    exp_table [339	] = 0.03370867297053337097167968750000000000000000000000f;
    exp_table [340	] = 0.03337326645851135253906250000000000000000000000000f;
    exp_table [341	] = 0.03304119780659675598144531250000000000000000000000f;
    exp_table [342	] = 0.03271243348717689514160156250000000000000000000000f;
    exp_table [343	] = 0.03238693997263908386230468750000000000000000000000f;
    exp_table [344	] = 0.03206468373537063598632812500000000000000000000000f;
    exp_table [345	] = 0.03174563497304916381835937500000000000000000000000f;
    exp_table [346	] = 0.03142976015806198120117187500000000000000000000000f;
    exp_table [347	] = 0.03111702948808670043945312500000000000000000000000f;
    exp_table [348	] = 0.03080741129815578460693359375000000000000000000000f;
    exp_table [349	] = 0.03050087206065654754638671875000000000000000000000f;
    exp_table [350	] = 0.03019738383591175079345703125000000000000000000000f;
    exp_table [351	] = 0.02989691495895385742187500000000000000000000000000f;
    exp_table [352	] = 0.02959943562746047973632812500000000000000000000000f;
    exp_table [353	] = 0.02930491603910923004150390625000000000000000000000f;
    exp_table [354	] = 0.02901332825422286987304687500000000000000000000000f;
    exp_table [355	] = 0.02872464060783386230468750000000000000000000000000f;
    exp_table [356	] = 0.02843882702291011810302734375000000000000000000000f;
    exp_table [357	] = 0.02815585583448410034179687500000000000000000000000f;
    exp_table [358	] = 0.02787570096552371978759765625000000000000000000000f;
    exp_table [359	] = 0.02759833261370658874511718750000000000000000000000f;
    exp_table [360	] = 0.02732372470200061798095703125000000000000000000000f;
    exp_table [361	] = 0.02705184929072856903076171875000000000000000000000f;
    exp_table [362	] = 0.02678268030285835266113281250000000000000000000000f;
    exp_table [363	] = 0.02651618048548698425292968750000000000000000000000f;
    exp_table [364	] = 0.02625234052538871765136718750000000000000000000000f;
    exp_table [365	] = 0.02599112689495086669921875000000000000000000000000f;
    exp_table [366	] = 0.02573250979185104370117187500000000000000000000000f;
    exp_table [367	] = 0.02547646872699260711669921875000000000000000000000f;
    exp_table [368	] = 0.02522297389805316925048828125000000000000000000000f;
    exp_table [369	] = 0.02497200109064579010009765625000000000000000000000f;
    exp_table [370	] = 0.02472352609038352966308593750000000000000000000000f;
    exp_table [371	] = 0.02447752282023429870605468750000000000000000000000f;
    exp_table [372	] = 0.02423396706581115722656250000000000000000000000000f;
    exp_table [373	] = 0.02399283461272716522216796875000000000000000000000f;
    exp_table [374	] = 0.02375410310924053192138671875000000000000000000000f;
    exp_table [375	] = 0.02351774647831916809082031250000000000000000000000f;
    exp_table [376	] = 0.02328374050557613372802734375000000000000000000000f;
    exp_table [377	] = 0.02305206283926963806152343750000000000000000000000f;
    exp_table [378	] = 0.02282269299030303955078125000000000000000000000000f;
    exp_table [379	] = 0.02259560301899909973144531250000000000000000000000f;
    exp_table [380	] = 0.02237077243626117706298828125000000000000000000000f;
    exp_table [381	] = 0.02214818075299263000488281250000000000000000000000f;
    exp_table [382	] = 0.02192780189216136932373046875000000000000000000000f;
    exp_table [383	] = 0.02170961722731590270996093750000000000000000000000f;
    exp_table [384	] = 0.02149360254406929016113281250000000000000000000000f;
    exp_table [385	] = 0.02127973921597003936767578125000000000000000000000f;
    exp_table [386	] = 0.02106800116598606109619140625000000000000000000000f;
    exp_table [387	] = 0.02085837163031101226806640625000000000000000000000f;
    exp_table [388	] = 0.02065082266926765441894531250000000000000000000000f;
    exp_table [389	] = 0.02044534310698509216308593750000000000000000000000f;
    exp_table [390	] = 0.02024190872907638549804687500000000000000000000000f;
    exp_table [391	] = 0.02004049904644489288330078125000000000000000000000f;
    exp_table [392	] = 0.01984109356999397277832031250000000000000000000000f;
    exp_table [393	] = 0.01964367181062698364257812500000000000000000000000f;
    exp_table [394	] = 0.01944821327924728393554687500000000000000000000000f;
    exp_table [395	] = 0.01925470121204853057861328125000000000000000000000f;
    exp_table [396	] = 0.01906311325728893280029296875000000000000000000000f;
    exp_table [397	] = 0.01887343265116214752197265625000000000000000000000f;
    exp_table [398	] = 0.01868563890457153320312500000000000000000000000000f;
    exp_table [399	] = 0.01849971339106559753417968750000000000000000000000f;
    exp_table [400	] = 0.01831563934683799743652343750000000000000000000000f;
    exp_table [401	] = 0.01813339069485664367675781250000000000000000000000f;
    exp_table [402	] = 0.01795296557247638702392578125000000000000000000000f;
    exp_table [403	] = 0.01777432672679424285888671875000000000000000000000f;
    exp_table [404	] = 0.01759747229516506195068359375000000000000000000000f;
    exp_table [405	] = 0.01742237061262130737304687500000000000000000000000f;
    exp_table [406	] = 0.01724901981651782989501953125000000000000000000000f;
    exp_table [407	] = 0.01707738637924194335937500000000000000000000000000f;
    exp_table [408	] = 0.01690746657550334930419921875000000000000000000000f;
    exp_table [409	] = 0.01673923060297966003417968750000000000000000000000f;
    exp_table [410	] = 0.01657267659902572631835937500000000000000000000000f;
    exp_table [411	] = 0.01640777289867401123046875000000000000000000000000f;
    exp_table [412	] = 0.01624451577663421630859375000000000000000000000000f;
    exp_table [413	] = 0.01608287729322910308837890625000000000000000000000f;
    exp_table [414	] = 0.01592285372316837310791015625000000000000000000000f;
    exp_table [415	] = 0.01576441526412963867187500000000000000000000000000f;
    exp_table [416	] = 0.01560756005346775054931640625000000000000000000000f;
    exp_table [417	] = 0.01545225922018289566040039062500000000000000000000f;
    exp_table [418	] = 0.01529850997030735015869140625000000000000000000000f;
    exp_table [419	] = 0.01514628436416387557983398437500000000000000000000f;
    exp_table [420	] = 0.01499557960778474807739257812500000000000000000000f;
    exp_table [421	] = 0.01484636776149272918701171875000000000000000000000f;
    exp_table [422	] = 0.01469864789396524429321289062500000000000000000000f;
    exp_table [423	] = 0.01455239020287990570068359375000000000000000000000f;
    exp_table [424	] = 0.01440759468823671340942382812500000000000000000000f;
    exp_table [425	] = 0.01426423434168100357055664062500000000000000000000f;
    exp_table [426	] = 0.01412229891866445541381835937500000000000000000000f;
    exp_table [427	] = 0.01398178376257419586181640625000000000000000000000f;
    exp_table [428	] = 0.01384265907108783721923828125000000000000000000000f;
    exp_table [429	] = 0.01370492577552795410156250000000000000000000000000f;
    exp_table [430	] = 0.01356855686753988265991210937500000000000000000000f;
    exp_table [431	] = 0.01343355048447847366333007812500000000000000000000f;
    exp_table [432	] = 0.01329988148063421249389648437500000000000000000000f;
    exp_table [433	] = 0.01316754892468452453613281250000000000000000000000f;
    exp_table [434	] = 0.01303652580827474594116210937500000000000000000000f;
    exp_table [435	] = 0.01290681399405002593994140625000000000000000000000f;
    exp_table [436	] = 0.01277838554233312606811523437500000000000000000000f;
    exp_table [437	] = 0.01265124231576919555664062500000000000000000000000f;
    exp_table [438	] = 0.01252535730600357055664062500000000000000000000000f;
    exp_table [439	] = 0.01240073051303625106811523437500000000000000000000f;
    exp_table [440	] = 0.01227733865380287170410156250000000000000000000000f;
    exp_table [441	] = 0.01215517986565828323364257812500000000000000000000f;
    exp_table [442	] = 0.01203423179686069488525390625000000000000000000000f;
    exp_table [443	] = 0.01191449165344238281250000000000000000000000000000f;
    exp_table [444	] = 0.01179593801498413085937500000000000000000000000000f;
    exp_table [445	] = 0.01167856901884078979492187500000000000000000000000f;
    exp_table [446	] = 0.01156236324459314346313476562500000000000000000000f;
    exp_table [447	] = 0.01144731789827346801757812500000000000000000000000f;
    exp_table [448	] = 0.01133341249078512191772460937500000000000000000000f;
    exp_table [449	] = 0.01122064609080553054809570312500000000000000000000f;
    exp_table [450	] = 0.01110899634659290313720703125000000000000000000000f;
    exp_table [451	] = 0.01099845767021179199218750000000000000000000000000f;
    exp_table [452	] = 0.01088902354240417480468750000000000000000000000000f;
    exp_table [453	] = 0.01078067347407341003417968750000000000000000000000f;
    exp_table [454	] = 0.01067340653389692306518554687500000000000000000000f;
    exp_table [455	] = 0.01056720223277807235717773437500000000000000000000f;
    exp_table [456	] = 0.01046205963939428329467773437500000000000000000000f;
    exp_table [457	] = 0.01035795826464891433715820312500000000000000000000f;
    exp_table [458	] = 0.01025489717721939086914062500000000000000000000000f;
    exp_table [459	] = 0.01015285681933164596557617187500000000000000000000f;
    exp_table [460	] = 0.01005183625966310501098632812500000000000000000000f;
    exp_table [461	] = 0.00995181687176227569580078125000000000000000000000f;
    exp_table [462	] = 0.00985279679298400878906250000000000000000000000000f;
    exp_table [463	] = 0.00975475832819938659667968750000000000000000000000f;
    exp_table [464	] = 0.00965769868344068527221679687500000000000000000000f;
    exp_table [465	] = 0.00956160109490156173706054687500000000000000000000f;
    exp_table [466	] = 0.00946646369993686676025390625000000000000000000000f;
    exp_table [467	] = 0.00937226880341768264770507812500000000000000000000f;
    exp_table [468	] = 0.00927901547402143478393554687500000000000000000000f;
    exp_table [469	] = 0.00918668601661920547485351562500000000000000000000f;
    exp_table [470	] = 0.00909527856856584548950195312500000000000000000000f;
    exp_table [471	] = 0.00900477729737758636474609375000000000000000000000f;
    exp_table [472	] = 0.00891518034040927886962890625000000000000000000000f;
    exp_table [473	] = 0.00882647093385457992553710937500000000000000000000f;
    exp_table [474	] = 0.00873864814639091491699218750000000000000000000000f;
    exp_table [475	] = 0.00865169521421194076538085937500000000000000000000f;
    exp_table [476	] = 0.00856560748070478439331054687500000000000000000000f;
    exp_table [477	] = 0.00848038028925657272338867187500000000000000000000f;
    exp_table [478	] = 0.00839599687606096267700195312500000000000000000000f;
    exp_table [479	] = 0.00831245724111795425415039062500000000000000000000f;
    exp_table [480	] = 0.00822974555194377899169921875000000000000000000000f;
    exp_table [481	] = 0.00814785994589328765869140625000000000000000000000f;
    exp_table [482	] = 0.00806678552180528640747070312500000000000000000000f;
    exp_table [483	] = 0.00798652227967977523803710937500000000000000000000f;
    exp_table [484	] = 0.00790705252438783645629882812500000000000000000000f;
    exp_table [485	] = 0.00782837811857461929321289062500000000000000000000f;
    exp_table [486	] = 0.00775048276409506797790527343750000000000000000000f;
    exp_table [487	] = 0.00767336599528789520263671875000000000000000000000f;
    exp_table [488	] = 0.00759701337665319442749023437500000000000000000000f;
    exp_table [489	] = 0.00752142351120710372924804687500000000000000000000f;
    exp_table [490	] = 0.00744658242911100387573242187500000000000000000000f;
    exp_table [491	] = 0.00737248966470360755920410156250000000000000000000f;
    exp_table [492	] = 0.00729913031682372093200683593750000000000000000000f;
    exp_table [493	] = 0.00722650438547134399414062500000000000000000000000f;
    exp_table [494	] = 0.00715459790080785751342773437500000000000000000000f;
    exp_table [495	] = 0.00708341039717197418212890625000000000000000000000f;
    exp_table [496	] = 0.00701292743906378746032714843750000000000000000000f;
    exp_table [497	] = 0.00694314949214458465576171875000000000000000000000f;
    exp_table [498	] = 0.00687406258657574653625488281250000000000000000000f;
    exp_table [499	] = 0.00680566625669598579406738281250000000000000000000f;
    exp_table [500	] = 0.00673794699832797050476074218750000000000000000000f;
    exp_table [501	] = 0.00667090155184268951416015625000000000000000000000f;
    exp_table [502	] = 0.00660452665761113166809082031250000000000000000000f;
    exp_table [503	] = 0.00653880927711725234985351562500000000000000000000f;
    exp_table [504	] = 0.00647374847903847694396972656250000000000000000000f;
    exp_table [505	] = 0.00640933215618133544921875000000000000000000000000f;
    exp_table [506	] = 0.00634555984288454055786132812500000000000000000000f;
    exp_table [507	] = 0.00628241896629333496093750000000000000000000000000f;
    exp_table [508	] = 0.00621990952640771865844726562500000000000000000000f;
    exp_table [509	] = 0.00615801895037293434143066406250000000000000000000f;
    exp_table [510	] = 0.00609674723818898200988769531250000000000000000000f;
    exp_table [511	] = 0.00603608228266239166259765625000000000000000000000f;
    exp_table [512	] = 0.00597602361813187599182128906250000000000000000000f;
    exp_table [513	] = 0.00591655960306525230407714843750000000000000000000f;
    exp_table [514	] = 0.00585769070312380790710449218750000000000000000000f;
    exp_table [515	] = 0.00579940434545278549194335937500000000000000000000f;
    exp_table [516	] = 0.00574170053005218505859375000000000000000000000000f;
    exp_table [517	] = 0.00568456854671239852905273437500000000000000000000f;
    exp_table [518	] = 0.00562800746411085128784179687500000000000000000000f;
    exp_table [519	] = 0.00557200657203793525695800781250000000000000000000f;
    exp_table [520	] = 0.00551656540483236312866210937500000000000000000000f;
    exp_table [521	] = 0.00546167325228452682495117187500000000000000000000f;
    exp_table [522	] = 0.00540733011439442634582519531250000000000000000000f;
    exp_table [523	] = 0.00535352528095245361328125000000000000000000000000f;
    exp_table [524	] = 0.00530025782063603401184082031250000000000000000000f;
    exp_table [525	] = 0.00524751842021942138671875000000000000000000000000f;
    exp_table [526	] = 0.00519530335441231727600097656250000000000000000000f;
    exp_table [527	] = 0.00514361076056957244873046875000000000000000000000f;
    exp_table [528	] = 0.00509242992848157882690429687500000000000000000000f;
    exp_table [529	] = 0.00504176039248704910278320312500000000000000000000f;
    exp_table [530	] = 0.00499159283936023712158203125000000000000000000000f;
    exp_table [531	] = 0.00494192680343985557556152343750000000000000000000f;
    exp_table [532	] = 0.00489275297150015830993652343750000000000000000000f;
    exp_table [533	] = 0.00484407041221857070922851562500000000000000000000f;
    exp_table [534	] = 0.00479586981236934661865234375000000000000000000000f;
    exp_table [535	] = 0.00474815163761377334594726562500000000000000000000f;
    exp_table [536	] = 0.00470090564340353012084960937500000000000000000000f;
    exp_table [537	] = 0.00465413182973861694335937500000000000000000000000f;
    exp_table [538	] = 0.00460782134905457496643066406250000000000000000000f;
    exp_table [539	] = 0.00456197373569011688232421875000000000000000000000f;
    exp_table [540	] = 0.00451658060774207115173339843750000000000000000000f;
    exp_table [541	] = 0.00447164103388786315917968750000000000000000000000f;
    exp_table [542	] = 0.00442714616656303405761718750000000000000000000000f;
    exp_table [543	] = 0.00438309647142887115478515625000000000000000000000f;
    exp_table [544	] = 0.00433948310092091560363769531250000000000000000000f;
    exp_table [545	] = 0.00429630558937788009643554687500000000000000000000f;
    exp_table [546	] = 0.00425355555489659309387207031250000000000000000000f;
    exp_table [547	] = 0.00421123299747705459594726562500000000000000000000f;
    exp_table [548	] = 0.00416932953521609306335449218750000000000000000000f;
    exp_table [549	] = 0.00412784516811370849609375000000000000000000000000f;
    exp_table [550	] = 0.00408677151426672935485839843750000000000000000000f;
    exp_table [551	] = 0.00404610624536871910095214843750000000000000000000f;
    exp_table [552	] = 0.00400584796443581581115722656250000000000000000000f;
    exp_table [553	] = 0.00396598828956484794616699218750000000000000000000f;
    exp_table [554	] = 0.00392652722075581550598144531250000000000000000000f;
    exp_table [555	] = 0.00388745660893619060516357421875000000000000000000f;
    exp_table [556	] = 0.00384877668693661689758300781250000000000000000000f;
    exp_table [557	] = 0.00381047977134585380554199218750000000000000000000f;
    exp_table [558	] = 0.00377256586216390132904052734375000000000000000000f;
    exp_table [559	] = 0.00373502727597951889038085937500000000000000000000f;
    exp_table [560	] = 0.00369786401279270648956298828125000000000000000000f;
    exp_table [561	] = 0.00366106885485351085662841796875000000000000000000f;
    exp_table [562	] = 0.00362464156933128833770751953125000000000000000000f;
    exp_table [563	] = 0.00358857493847608566284179687500000000000000000000f;
    exp_table [564	] = 0.00355286896228790283203125000000000000000000000000f;
    exp_table [565	] = 0.00351751642301678657531738281250000000000000000000f;
    exp_table [566	] = 0.00348251732066273689270019531250000000000000000000f;
    exp_table [567	] = 0.00344786490313708782196044921875000000000000000000f;
    exp_table [568	] = 0.00341355893760919570922851562500000000000000000000f;
    exp_table [569	] = 0.00337959267199039459228515625000000000000000000000f;
    exp_table [570	] = 0.00334596610628068447113037109375000000000000000000f;
    exp_table [571	] = 0.00331267248839139938354492187500000000000000000000f;
    exp_table [572	] = 0.00327971158549189567565917968750000000000000000000f;
    exp_table [573	] = 0.00324707711115479469299316406250000000000000000000f;
    exp_table [574	] = 0.00321476906538009643554687500000000000000000000000f;
    exp_table [575	] = 0.00318278069607913494110107421875000000000000000000f;
    exp_table [576	] = 0.00315111083909869194030761718750000000000000000000f;
    exp_table [577	] = 0.00311975763179361820220947265625000000000000000000f;
    exp_table [578	] = 0.00308871478773653507232666015625000000000000000000f;
    exp_table [579	] = 0.00305798230692744255065917968750000000000000000000f;
    exp_table [580	] = 0.00302755413576960563659667968750000000000000000000f;
    exp_table [581	] = 0.00299743027426302433013916015625000000000000000000f;
    exp_table [582	] = 0.00296760466881096363067626953125000000000000000000f;
    exp_table [583	] = 0.00293807731941342353820800781250000000000000000000f;
    exp_table [584	] = 0.00290884217247366905212402343750000000000000000000f;
    exp_table [585	] = 0.00287989946082234382629394531250000000000000000000f;
    exp_table [586	] = 0.00285124336369335651397705078125000000000000000000f;
    exp_table [587	] = 0.00282287364825606346130371093750000000000000000000f;
    exp_table [588	] = 0.00279478495940566062927246093750000000000000000000f;
    exp_table [589	] = 0.00276697706431150436401367187500000000000000000000f;
    exp_table [590	] = 0.00273944460786879062652587890625000000000000000000f;
    exp_table [591	] = 0.00271218735724687576293945312500000000000000000000f;
    exp_table [592	] = 0.00268519995734095573425292968750000000000000000000f;
    exp_table [593	] = 0.00265848240815103054046630859375000000000000000000f;
    exp_table [594	] = 0.00263202958740293979644775390625000000000000000000f;
    exp_table [595	] = 0.00260584102943539619445800781250000000000000000000f;
    exp_table [596	] = 0.00257991184480488300323486328125000000000000000000f;
    exp_table [597	] = 0.00255424203351140022277832031250000000000000000000f;
    exp_table [598	] = 0.00252882624045014381408691406250000000000000000000f;
    exp_table [599	] = 0.00250366469845175743103027343750000000000000000000f;
    exp_table [600	] = 0.00247875228524208068847656250000000000000000000000f;
    exp_table [601	] = 0.00245408760383725166320800781250000000000000000000f;
    exp_table [602	] = 0.00242966972291469573974609375000000000000000000000f;
    exp_table [603	] = 0.00240549352020025253295898437500000000000000000000f;
    exp_table [604	] = 0.00238155899569392204284667968750000000000000000000f;
    exp_table [605	] = 0.00235786149278283119201660156250000000000000000000f;
    exp_table [606	] = 0.00233440101146697998046875000000000000000000000000f;
    exp_table [607	] = 0.00231117289513349533081054687500000000000000000000f;
    exp_table [608	] = 0.00228817691095173358917236328125000000000000000000f;
    exp_table [609	] = 0.00226540863513946533203125000000000000000000000000f;
    exp_table [610	] = 0.00224286783486604690551757812500000000000000000000f;
    exp_table [611	] = 0.00222055055201053619384765625000000000000000000000f;
    exp_table [612	] = 0.00219845632091164588928222656250000000000000000000f;
    exp_table [613	] = 0.00217658071778714656829833984375000000000000000000f;
    exp_table [614	] = 0.00215492397546768188476562500000000000000000000000f;
    exp_table [615	] = 0.00213348167017102241516113281250000000000000000000f;
    exp_table [616	] = 0.00211225356906652450561523437500000000000000000000f;
    exp_table [617	] = 0.00209123594686388969421386718750000000000000000000f;
    exp_table [618	] = 0.00207042810507118701934814453125000000000000000000f;
    exp_table [619	] = 0.00204982655122876167297363281250000000000000000000f;
    exp_table [620	] = 0.00202943105250597000122070312500000000000000000000f;
    exp_table [621	] = 0.00200923741795122623443603515625000000000000000000f;
    exp_table [622	] = 0.00198924564756453037261962890625000000000000000000f;
    exp_table [623	] = 0.00196945178322494029998779296875000000000000000000f;
    exp_table [624	] = 0.00194985594134777784347534179687500000000000000000f;
    exp_table [625	] = 0.00193045416381210088729858398437500000000000000000f;
    exp_table [626	] = 0.00191124540288001298904418945312500000000000000000f;
    exp_table [627	] = 0.00189222861081361770629882812500000000000000000000f;
    exp_table [628	] = 0.00187340017873793840408325195312500000000000000000f;
    exp_table [629	] = 0.00185475999023765325546264648437500000000000000000f;
    exp_table [630	] = 0.00183630443643778562545776367187500000000000000000f;
    exp_table [631	] = 0.00181803340092301368713378906250000000000000000000f;
    exp_table [632	] = 0.00179994315840303897857666015625000000000000000000f;
    exp_table [633	] = 0.00178203394170850515365600585937500000000000000000f;
    exp_table [634	] = 0.00176430202554911375045776367187500000000000000000f;
    exp_table [635	] = 0.00174674729350954294204711914062500000000000000000f;
    exp_table [636	] = 0.00172936648596078157424926757812500000000000000000f;
    exp_table [637	] = 0.00171215937007218599319458007812500000000000000000f;
    exp_table [638	] = 0.00169512280263006687164306640625000000000000000000f;
    exp_table [639	] = 0.00167825643438845872879028320312500000000000000000f;
    exp_table [640	] = 0.00166155712213367223739624023437500000000000000000f;
    exp_table [641	] = 0.00164502474945038557052612304687500000000000000000f;
    exp_table [642	] = 0.00162865605670958757400512695312500000000000000000f;
    exp_table [643	] = 0.00161245116032660007476806640625000000000000000000f;
    exp_table [644	] = 0.00159640656784176826477050781250000000000000000000f;
    exp_table [645	] = 0.00158052251208573579788208007812500000000000000000f;
    exp_table [646	] = 0.00156479561701416969299316406250000000000000000000f;
    exp_table [647	] = 0.00154922611545771360397338867187500000000000000000f;
    exp_table [648	] = 0.00153381063137203454971313476562500000000000000000f;
    exp_table [649	] = 0.00151854939758777618408203125000000000000000000000f;
    exp_table [650	] = 0.00150343915447592735290527343750000000000000000000f;
    exp_table [651	] = 0.00148847943637520074844360351562500000000000000000f;
    exp_table [652	] = 0.00147366907913237810134887695312500000000000000000f;
    exp_table [653	] = 0.00145900552161037921905517578125000000000000000000f;
    exp_table [654	] = 0.00144448853097856044769287109375000000000000000000f;
    exp_table [655	] = 0.00143011531326919794082641601562500000000000000000f;
    exp_table [656	] = 0.00141588575206696987152099609375000000000000000000f;
    exp_table [657	] = 0.00140179716981947422027587890625000000000000000000f;
    exp_table [658	] = 0.00138784945011138916015625000000000000000000000000f;
    exp_table [659	] = 0.00137403979897499084472656250000000000000000000000f;
    exp_table [660	] = 0.00136036821641027927398681640625000000000000000000f;
    exp_table [661	] = 0.00134683202486485242843627929687500000000000000000f;
    exp_table [662	] = 0.00133343110792338848114013671875000000000000000000f;
    exp_table [663	] = 0.00132016290444880723953247070312500000000000000000f;
    exp_table [664	] = 0.00130702741444110870361328125000000000000000000000f;
    exp_table [665	] = 0.00129402196034789085388183593750000000000000000000f;
    exp_table [666	] = 0.00128114654216915369033813476562500000000000000000f;
    exp_table [667	] = 0.00126839859876781702041625976562500000000000000000f;
    exp_table [668	] = 0.00125577813014388084411621093750000000000000000000f;
    exp_table [669	] = 0.00124328269157558679580688476562500000000000000000f;
    exp_table [670	] = 0.00123091216664761304855346679687500000000000000000f;
    exp_table [671	] = 0.00121866411063820123672485351562500000000000000000f;
    exp_table [672	] = 0.00120653852354735136032104492187500000000000000000f;
    exp_table [673	] = 0.00119453296065330505371093750000000000000000000000f;
    exp_table [674	] = 0.00118264742195606231689453125000000000000000000000f;
    exp_table [675	] = 0.00117087957914918661117553710937500000000000000000f;
    exp_table [676	] = 0.00115922896657139062881469726562500000000000000000f;
    exp_table [677	] = 0.00114769465290009975433349609375000000000000000000f;
    exp_table [678	] = 0.00113627465907484292984008789062500000000000000000f;
    exp_table [679	] = 0.00112496886868029832839965820312500000000000000000f;
    exp_table [680	] = 0.00111377495341002941131591796875000000000000000000f;
    exp_table [681	] = 0.00110269291326403617858886718750000000000000000000f;
    exp_table [682	] = 0.00109172076918184757232666015625000000000000000000f;
    exp_table [683	] = 0.00108085817191749811172485351562500000000000000000f;
    exp_table [684	] = 0.00107010325882583856582641601562500000000000000000f;
    exp_table [685	] = 0.00105945579707622528076171875000000000000000000000f;
    exp_table [686	] = 0.00104891380760818719863891601562500000000000000000f;
    exp_table [687	] = 0.00103847717400640249252319335937500000000000000000f;
    exp_table [688	] = 0.00102814391721040010452270507812500000000000000000f;
    exp_table [689	] = 0.00101791392080485820770263671875000000000000000000f;
    exp_table [690	] = 0.00100778532214462757110595703125000000000000000000f;
    exp_table [691	] = 0.00099775800481438636779785156250000000000000000000f;
    exp_table [692	] = 0.00098782987333834171295166015625000000000000000000f;
    exp_table [693	] = 0.00097800104413181543350219726562500000000000000000f;
    exp_table [694	] = 0.00096826953813433647155761718750000000000000000000f;
    exp_table [695	] = 0.00095863535534590482711791992187500000000000000000f;
    exp_table [696	] = 0.00094909651670604944229125976562500000000000000000f;
    exp_table [697	] = 0.00093965308042243123054504394531250000000000000000f;
    exp_table [698	] = 0.00093030318384990096092224121093750000000000000000f;
    exp_table [699	] = 0.00092104676878079771995544433593750000000000000000f;
    exp_table [700	] = 0.00091188197256997227668762207031250000000000000000f;
    exp_table [701	] = 0.00090280838776379823684692382812500000000000000000f;
    exp_table [702	] = 0.00089382549049332737922668457031250000000000000000f;
    exp_table [703	] = 0.00088493159273639321327209472656250000000000000000f;
    exp_table [704	] = 0.00087612657807767391204833984375000000000000000000f;
    exp_table [705	] = 0.00086740881670266389846801757812500000000000000000f;
    exp_table [706	] = 0.00085877813398838043212890625000000000000000000000f;
    exp_table [707	] = 0.00085023295832797884941101074218750000000000000000f;
    exp_table [708	] = 0.00084177323151379823684692382812500000000000000000f;
    exp_table [709	] = 0.00083339726552367210388183593750000000000000000000f;
    exp_table [710	] = 0.00082510500214993953704833984375000000000000000000f;
    exp_table [711	] = 0.00081689486978575587272644042968750000000000000000f;
    exp_table [712	] = 0.00080876686843112111091613769531250000000000000000f;
    exp_table [713	] = 0.00080071931006386876106262207031250000000000000000f;
    exp_table [714	] = 0.00079275219468399882316589355468750000000000000000f;
    exp_table [715	] = 0.00078486400889232754707336425781250000000000000000f;
    exp_table [716	] = 0.00077705469448119401931762695312500000000000000000f;
    exp_table [717	] = 0.00076932267984375357627868652343750000000000000000f;
    exp_table [718	] = 0.00076166796498000621795654296875000000000000000000f;
    exp_table [719	] = 0.00075408909469842910766601562500000000000000000000f;
    exp_table [720	] = 0.00074658595258370041847229003906250000000000000000f;
    exp_table [721	] = 0.00073915714165195822715759277343750000000000000000f;
    exp_table [722	] = 0.00073180254548788070678710937500000000000000000000f;
    exp_table [723	] = 0.00072452082531526684761047363281250000000000000000f;
    exp_table [724	] = 0.00071731192292645573616027832031250000000000000000f;
    exp_table [725	] = 0.00071017438312992453575134277343750000000000000000f;
    exp_table [726	] = 0.00070310785667970776557922363281250000000000000000f;
    exp_table [727	] = 0.00069611199432983994483947753906250000000000000000f;
    exp_table [728	] = 0.00068918539909645915031433105468750000000000000000f;
    exp_table [729	] = 0.00068232807097956538200378417968750000000000000000f;
    exp_table [730	] = 0.00067553867120295763015747070312500000000000000000f;
    exp_table [731	] = 0.00066881708335131406784057617187500000000000000000f;
    exp_table [732	] = 0.00066216208506375551223754882812500000000000000000f;
    exp_table [733	] = 0.00065557361813262104988098144531250000000000000000f;
    exp_table [734	] = 0.00064905040198937058448791503906250000000000000000f;
    exp_table [735	] = 0.00064259243663400411605834960937500000000000000000f;
    exp_table [736	] = 0.00063619838329032063484191894531250000000000000000f;
    exp_table [737	] = 0.00062986824195832014083862304687500000000000000000f;
    exp_table [738	] = 0.00062360079027712345123291015625000000000000000000f;
    exp_table [739	] = 0.00061739602824673056602478027343750000000000000000f;
    exp_table [740	] = 0.00061125267529860138893127441406250000000000000000f;
    exp_table [741	] = 0.00060517078964039683341979980468750000000000000000f;
    exp_table [742	] = 0.00059914909070357680320739746093750000000000000000f;
    exp_table [743	] = 0.00059318763669580221176147460937500000000000000000f;
    exp_table [744	] = 0.00058728514704853296279907226562500000000000000000f;
    exp_table [745	] = 0.00058144173817709088325500488281250000000000000000f;
    exp_table [746	] = 0.00057565612951293587684631347656250000000000000000f;
    exp_table [747	] = 0.00056992843747138977050781250000000000000000000000f;
    exp_table [748	] = 0.00056425738148391246795654296875000000000000000000f;
    exp_table [749	] = 0.00055864307796582579612731933593750000000000000000f;
    exp_table [750	] = 0.00055308436276391148567199707031250000000000000000f;
    exp_table [751	] = 0.00054758094483986496925354003906250000000000000000f;
    exp_table [752	] = 0.00054213259136304259300231933593750000000000000000f;
    exp_table [753	] = 0.00053673813818022608757019042968750000000000000000f;
    exp_table [754	] = 0.00053139764349907636642456054687500000000000000000f;
    exp_table [755	] = 0.00052611000137403607368469238281250000000000000000f;
    exp_table [756	] = 0.00052087527001276612281799316406250000000000000000f;
    exp_table [757	] = 0.00051569234346970915794372558593750000000000000000f;
    exp_table [758	] = 0.00051056127995252609252929687500000000000000000000f;
    exp_table [759	] = 0.00050548097351565957069396972656250000000000000000f;
    exp_table [760	] = 0.00050045148236677050590515136718750000000000000000f;
    exp_table [761	] = 0.00049547181697562336921691894531250000000000000000f;
    exp_table [762	] = 0.00049054186092689633369445800781250000000000000000f;
    exp_table [763	] = 0.00048566079931333661079406738281250000000000000000f;
    exp_table [764	] = 0.00048082851571962237358093261718750000000000000000f;
    exp_table [765	] = 0.00047604407882317900657653808593750000000000000000f;
    exp_table [766	] = 0.00047130748862400650978088378906250000000000000000f;
    exp_table [767	] = 0.00046661778469569981098175048828125000000000000000f;
    exp_table [768	] = 0.00046197496703825891017913818359375000000000000000f;
    exp_table [769	] = 0.00045737813343293964862823486328125000000000000000f;
    exp_table [770	] = 0.00045282725477591156959533691406250000000000000000f;
    exp_table [771	] = 0.00044832145795226097106933593750000000000000000000f;
    exp_table [772	] = 0.00044386068475432693958282470703125000000000000000f;
    exp_table [773	] = 0.00043944412027485668659210205078125000000000000000f;
    exp_table [774	] = 0.00043507167720235884189605712890625000000000000000f;
    exp_table [775	] = 0.00043074254062958061695098876953125000000000000000f;
    exp_table [776	] = 0.00042645647772587835788726806640625000000000000000f;
    exp_table [777	] = 0.00042221328476443886756896972656250000000000000000f;
    exp_table [778	] = 0.00041801208863034844398498535156250000000000000000f;
    exp_table [779	] = 0.00041385288932360708713531494140625000000000000000f;
    exp_table [780	] = 0.00040973490104079246520996093750000000000000000000f;
    exp_table [781	] = 0.00040565806557424366474151611328125000000000000000f;
    exp_table [782	] = 0.00040162162622436881065368652343750000000000000000f;
    exp_table [783	] = 0.00039762552478350698947906494140625000000000000000f;
    exp_table [784	] = 0.00039366897544823586940765380859375000000000000000f;
    exp_table [785	] = 0.00038975200732238590717315673828125000000000000000f;
    exp_table [786	] = 0.00038587380549870431423187255859375000000000000000f;
    exp_table [787	] = 0.00038203439908102154731750488281250000000000000000f;
    exp_table [788	] = 0.00037823300226591527462005615234375000000000000000f;
    exp_table [789	] = 0.00037446961505338549613952636718750000000000000000f;
    exp_table [790	] = 0.00037074350984767079353332519531250000000000000000f;
    exp_table [791	] = 0.00036705462844111025333404541015625000000000000000f;
    exp_table [792	] = 0.00036340230144560337066650390625000000000000000000f;
    exp_table [793	] = 0.00035978647065348923206329345703125000000000000000f;
    exp_table [794	] = 0.00035620646667666733264923095703125000000000000000f;
    exp_table [795	] = 0.00035266223130747675895690917968750000000000000000f;
    exp_table [796	] = 0.00034915309515781700611114501953125000000000000000f;
    exp_table [797	] = 0.00034567905822768807411193847656250000000000000000f;
    exp_table [798	] = 0.00034223942202515900135040283203125000000000000000f;
    exp_table [799	] = 0.00033883415744639933109283447265625000000000000000f;
    exp_table [800	] = 0.00033546262420713901519775390625000000000000000000f;
    exp_table [801	] = 0.00033212464768439531326293945312500000000000000000f;
    exp_table [802	] = 0.00032881987863220274448394775390625000000000000000f;
    exp_table [803	] = 0.00032554828794673085212707519531250000000000000000f;
    exp_table [804	] = 0.00032230897340923547744750976562500000000000000000f;
    exp_table [805	] = 0.00031910184770822525024414062500000000000000000000f;
    exp_table [806	] = 0.00031592667801305651664733886718750000000000000000f;
    exp_table [807	] = 0.00031278337701223790645599365234375000000000000000f;
    exp_table [808	] = 0.00030967107159085571765899658203125000000000000000f;
    exp_table [809	] = 0.00030658970354124903678894042968750000000000000000f;
    exp_table [810	] = 0.00030353901092894375324249267578125000000000000000f;
    exp_table [811	] = 0.00030051896465010941028594970703125000000000000000f;
    exp_table [812	] = 0.00029752869158983230590820312500000000000000000000f;
    exp_table [813	] = 0.00029456816264428198337554931640625000000000000000f;
    exp_table [814	] = 0.00029163708677515387535095214843750000000000000000f;
    exp_table [815	] = 0.00028873546398244798183441162109375000000000000000f;
    exp_table [816	] = 0.00028586245025508105754852294921875000000000000000f;
    exp_table [817	] = 0.00028301798738539218902587890625000000000000000000f;
    exp_table [818	] = 0.00028020184254273772239685058593750000000000000000f;
    exp_table [819	] = 0.00027741398662328720092773437500000000000000000000f;
    exp_table [820	] = 0.00027465363382361829280853271484375000000000000000f;
    exp_table [821	] = 0.00027192069683223962783813476562500000000000000000f;
    exp_table [822	] = 0.00026921500102616846561431884765625000000000000000f;
    exp_table [823	] = 0.00026653645909391343593597412109375000000000000000f;
    exp_table [824	] = 0.00026388431433588266372680664062500000000000000000f;
    exp_table [825	] = 0.00026125856675207614898681640625000000000000000000f;
    exp_table [826	] = 0.00025865892530418932437896728515625000000000000000f;
    exp_table [827	] = 0.00025608518626540899276733398437500000000000000000f;
    exp_table [828	] = 0.00025353726232424378395080566406250000000000000000f;
    exp_table [829	] = 0.00025101448409259319305419921875000000000000000000f;
    exp_table [830	] = 0.00024851679336279630661010742187500000000000000000f;
    exp_table [831	] = 0.00024604392820037901401519775390625000000000000000f;
    exp_table [832	] = 0.00024359593226108700037002563476562500000000000000f;
    exp_table [833	] = 0.00024117206339724361896514892578125000000000000000f;
    exp_table [834	] = 0.00023877230705693364143371582031250000000000000000f;
    exp_table [835	] = 0.00023639643040951341390609741210937500000000000000f;
    exp_table [836	] = 0.00023404441890306770801544189453125000000000000000f;
    exp_table [837	] = 0.00023171557404566556215286254882812500000000000000f;
    exp_table [838	] = 0.00022940992494113743305206298828125000000000000000f;
    exp_table [839	] = 0.00022712719510309398174285888671875000000000000000f;
    exp_table [840	] = 0.00022486741363536566495895385742187500000000000000f;
    exp_table [841	] = 0.00022262989659793674945831298828125000000000000000f;
    exp_table [842	] = 0.00022041462943889200687408447265625000000000000000f;
    exp_table [843	] = 0.00021822142298333346843719482421875000000000000000f;
    exp_table [844	] = 0.00021605024812743067741394042968750000000000000000f;
    exp_table [845	] = 0.00021390045003499835729598999023437500000000000000f;
    exp_table [846	] = 0.00021177205780986696481704711914062500000000000000f;
    exp_table [847	] = 0.00020966485317330807447433471679687500000000000000f;
    exp_table [848	] = 0.00020757879246957600116729736328125000000000000000f;
    exp_table [849	] = 0.00020551330817397683858871459960937500000000000000f;
    exp_table [850	] = 0.00020346837118268013000488281250000000000000000000f;
    exp_table [851	] = 0.00020144377776887267827987670898437500000000000000f;
    exp_table [852	] = 0.00019943933875765651464462280273437500000000000000f;
    exp_table [853	] = 0.00019745502504520118236541748046875000000000000000f;
    exp_table [854	] = 0.00019549026910681277513504028320312500000000000000f;
    exp_table [855	] = 0.00019354505639057606458663940429687500000000000000f;
    exp_table [856	] = 0.00019161921227350831031799316406250000000000000000f;
    exp_table [857	] = 0.00018971270765177905559539794921875000000000000000f;
    exp_table [858	] = 0.00018782498955260962247848510742187500000000000000f;
    exp_table [859	] = 0.00018595605797600001096725463867187500000000000000f;
    exp_table [860	] = 0.00018410572374705225229263305664062500000000000000f;
    exp_table [861	] = 0.00018227397231385111808776855468750000000000000000f;
    exp_table [862	] = 0.00018046027980744838714599609375000000000000000000f;
    exp_table [863	] = 0.00017866463167592883110046386718750000000000000000f;
    exp_table [864	] = 0.00017688683874439448118209838867187500000000000000f;
    exp_table [865	] = 0.00017512691556476056575775146484375000000000000000f;
    exp_table [866	] = 0.00017338433826807886362075805664062500000000000000f;
    exp_table [867	] = 0.00017165909230243414640426635742187500000000000000f;
    exp_table [868	] = 0.00016995101759675890207290649414062500000000000000f;
    exp_table [869	] = 0.00016826009959913790225982666015625000000000000000f;
    exp_table [870	] = 0.00016658584354445338249206542968750000000000000000f;
    exp_table [871	] = 0.00016492824943270534276962280273437500000000000000f;
    exp_table [872	] = 0.00016328714264091104269027709960937500000000000000f;
    exp_table [873	] = 0.00016166252316907048225402832031250000000000000000f;
    exp_table [874	] = 0.00016005392535589635372161865234375000000000000000f;
    exp_table [875	] = 0.00015846132009755820035934448242187500000000000000f;
    exp_table [876	] = 0.00015688457642681896686553955078125000000000000000f;
    exp_table [877	] = 0.00015532350516878068447113037109375000000000000000f;
    exp_table [878	] = 0.00015377812087535858154296875000000000000000000000f;
    exp_table [879	] = 0.00015224797243718057870864868164062500000000000000f;
    exp_table [880	] = 0.00015073304530233144760131835937500000000000000000f;
    exp_table [881	] = 0.00014923319395165890455245971679687500000000000000f;
    exp_table [882	] = 0.00014774840383324772119522094726562500000000000000f;
    exp_table [883	] = 0.00014627825294155627489089965820312500000000000000f;
    exp_table [884	] = 0.00014482272672466933727264404296875000000000000000f;
    exp_table [885	] = 0.00014338167966343462467193603515625000000000000000f;
    exp_table [886	] = 0.00014195511175785213708877563476562500000000000000f;
    exp_table [887	] = 0.00014054260100238025188446044921875000000000000000f;
    exp_table [888	] = 0.00013914414739701896905899047851562500000000000000f;
    exp_table [889	] = 0.00013775960542261600494384765625000000000000000000f;
    exp_table [890	] = 0.00013638897507917135953903198242187500000000000000f;
    exp_table [891	] = 0.00013503184891305863857269287109375000000000000000f;
    exp_table [892	] = 0.00013368824147619307041168212890625000000000000000f;
    exp_table [893	] = 0.00013235797814559191465377807617187500000000000000f;
    exp_table [894	] = 0.00013104108802508562803268432617187500000000000000f;
    exp_table [895	] = 0.00012973717821296304464340209960937500000000000000f;
    exp_table [896	] = 0.00012844624870922416448593139648437500000000000000f;
    exp_table [897	] = 0.00012716815399471670389175415039062500000000000000f;
    exp_table [898	] = 0.00012590290862135589122772216796875000000000000000f;
    exp_table [899	] = 0.00012465011968743056058883666992187500000000000000f;
    exp_table [900	] = 0.00012340980174485594034194946289062500000000000000f;
    exp_table [901	] = 0.00012218182382639497518539428710937500000000000000f;
    exp_table [902	] = 0.00012096606951672583818435668945312500000000000000f;
    exp_table [903	] = 0.00011976252426393330097198486328125000000000000000f;
    exp_table [904	] = 0.00011857083882205188274383544921875000000000000000f;
    exp_table [905	] = 0.00011739101319108158349990844726562500000000000000f;
    exp_table [906	] = 0.00011622293095570057630538940429687500000000000000f;
    exp_table [907	] = 0.00011506657028803601861000061035156250000000000000f;
    exp_table [908	] = 0.00011392161832191050052642822265625000000000000000f;
    exp_table [909	] = 0.00011278805322945117950439453125000000000000000000f;
    exp_table [910	] = 0.00011166576587129384279251098632812500000000000000f;
    exp_table [911	] = 0.00011055475624743849039077758789062500000000000000f;
    exp_table [912	] = 0.00010945468966383486986160278320312500000000000000f;
    exp_table [913	] = 0.00010836557339644059538841247558593750000000000000f;
    exp_table [914	] = 0.00010728729102993384003639221191406250000000000000f;
    exp_table [915	] = 0.00010621984256431460380554199218750000000000000000f;
    exp_table [916	] = 0.00010516291513340547680854797363281250000000000000f;
    exp_table [917	] = 0.00010411650146124884486198425292968750000000000000f;
    exp_table [918	] = 0.00010308049968443810939788818359375000000000000000f;
    exp_table [919	] = 0.00010205490980297327041625976562500000000000000000f;
    exp_table [920	] = 0.00010103941895067691802978515625000000000000000000f;
    exp_table [921	] = 0.00010003404167946428060531616210937500000000000000f;
    exp_table [922	] = 0.00009903866157401353120803833007812500000000000000f;
    exp_table [923	] = 0.00009805327863432466983795166015625000000000000000f;
    exp_table [924	] = 0.00009707760909805074334144592285156250000000000000f;
    exp_table [925	] = 0.00009611165296519175171852111816406250000000000000f;
    exp_table [926	] = 0.00009515530109638348221778869628906250000000000000f;
    exp_table [927	] = 0.00009420847345609217882156372070312500000000000000f;
    exp_table [928	] = 0.00009327114821644499897956848144531250000000000000f;
    exp_table [929	] = 0.00009234306344296783208847045898437500000000000000f;
    exp_table [930	] = 0.00009142421185970306396484375000000000000000000000f;
    exp_table [931	] = 0.00009051450615515932440757751464843750000000000000f;
    exp_table [932	] = 0.00008961393905337899923324584960937500000000000000f;
    exp_table [933	] = 0.00008872224134393036365509033203125000000000000000f;
    exp_table [934	] = 0.00008783942030277103185653686523437500000000000000f;
    exp_table [935	] = 0.00008696538861840963363647460937500000000000000000f;
    exp_table [936	] = 0.00008610013173893094062805175781250000000000000000f;
    exp_table [937	] = 0.00008524339500581845641136169433593750000000000000f;
    exp_table [938	] = 0.00008439519297098740935325622558593750000000000000f;
    exp_table [939	] = 0.00008355543104698881506919860839843750000000000000f;
    exp_table [940	] = 0.00008272409468190744519233703613281250000000000000f;
    exp_table [941	] = 0.00008190095832105726003646850585937500000000000000f;
    exp_table [942	] = 0.00008108601468848064541816711425781250000000000000f;
    exp_table [943	] = 0.00008027917647268623113632202148437500000000000000f;
    exp_table [944	] = 0.00007948044367367401719093322753906250000000000000f;
    exp_table [945	] = 0.00007868958346080034971237182617187500000000000000f;
    exp_table [946	] = 0.00007790658855810761451721191406250000000000000000f;
    exp_table [947	] = 0.00007713138620601966977119445800781250000000000000f;
    exp_table [948	] = 0.00007636396912857890129089355468750000000000000000f;
    exp_table [949	] = 0.00007560411904705688357353210449218750000000000000f;
    exp_table [950	] = 0.00007485182868549600243568420410156250000000000000f;
    exp_table [951	] = 0.00007410702528432011604309082031250000000000000000f;
    exp_table [952	] = 0.00007336962880799546837806701660156250000000000000f;
    exp_table [953	] = 0.00007263964653247967362403869628906250000000000000f;
    exp_table [954	] = 0.00007191685290308669209480285644531250000000000000f;
    exp_table [955	] = 0.00007120124791981652379035949707031250000000000000f;
    exp_table [956	] = 0.00007049276609905064105987548828125000000000000000f;
    exp_table [957	] = 0.00006979140744078904390335083007812500000000000000f;
    exp_table [958	] = 0.00006909695366630330681800842285156250000000000000f;
    exp_table [959	] = 0.00006840941205155104398727416992187500000000000000f;
    exp_table [960	] = 0.00006772870983695611357688903808593750000000000000f;
    exp_table [961	] = 0.00006705484702251851558685302734375000000000000000f;
    exp_table [962	] = 0.00006638762715738266706466674804687500000000000000f;
    exp_table [963	] = 0.00006572704296559095382690429687500000000000000000f;
    exp_table [964	] = 0.00006507302896352484822273254394531250000000000000f;
    exp_table [965	] = 0.00006442559242714196443557739257812500000000000000f;
    exp_table [966	] = 0.00006378452962962910532951354980468750000000000000f;
    exp_table [967	] = 0.00006314984784694388508796691894531250000000000000f;
    exp_table [968	] = 0.00006252148159546777606010437011718750000000000000f;
    exp_table [969	] = 0.00006189943087520077824592590332031250000000000000f;
    exp_table [970	] = 0.00006128350651124492287635803222656250000000000000f;
    exp_table [971	] = 0.00006067371214157901704311370849609375000000000000f;
    exp_table [972	] = 0.00006006998592056334018707275390625000000000000000f;
    exp_table [973	] = 0.00005947232057224027812480926513671875000000000000f;
    exp_table [974	] = 0.00005888054874958470463752746582031250000000000000f;
    exp_table [975	] = 0.00005829466317663900554180145263671875000000000000f;
    exp_table [976	] = 0.00005771460928372107446193695068359375000000000000f;
    exp_table [977	] = 0.00005714032522519119083881378173828125000000000000f;
    exp_table [978	] = 0.00005657181100104935467243194580078125000000000000f;
    exp_table [979	] = 0.00005600889926427043974399566650390625000000000000f;
    exp_table [980	] = 0.00005545159001485444605350494384765625000000000000f;
    exp_table [981	] = 0.00005489982504514046013355255126953125000000000000f;
    exp_table [982	] = 0.00005435360071714967489242553710937500000000000000f;
    exp_table [983	] = 0.00005381276059779338538646697998046875000000000000f;
    exp_table [984	] = 0.00005327730468707159161567687988281250000000000000f;
    exp_table [985	] = 0.00005274717113934457302093505859375000000000000000f;
    exp_table [986	] = 0.00005222236723056994378566741943359375000000000000f;
    exp_table [987	] = 0.00005170273288968019187450408935546875000000000000f;
    exp_table [988	] = 0.00005118827175465412437915802001953125000000000000f;
    exp_table [989	] = 0.00005067892925580963492393493652343750000000000000f;
    exp_table [990	] = 0.00005017470175516791641712188720703125000000000000f;
    exp_table [991	] = 0.00004967544373357668519020080566406250000000000000f;
    exp_table [992	] = 0.00004918115155305713415145874023437500000000000000f;
    exp_table [993	] = 0.00004869178155786357820034027099609375000000000000f;
    exp_table [994	] = 0.00004820732647203840315341949462890625000000000000f;
    exp_table [995	] = 0.00004772764441440813243389129638671875000000000000f;
    exp_table [996	] = 0.00004725273538497276604175567626953125000000000000f;
    exp_table [997	] = 0.00004678255209000781178474426269531250000000000000f;
    exp_table [998	] = 0.00004631709089153446257114410400390625000000000000f;
    exp_table [999	] = 0.00004585621718433685600757598876953125000000000000f;

    int inx = (int) ((-input)*100.0f);

    return exp_table[inx];
}





int pe_0(
        
        float pos_x_i,
        float pos_y_i,
        float pos_z_i,
        float pos_x_j,
        float pos_y_j,
        float pos_z_j,
        
        float * r_ij_x,
        float * r_ij_y,
        float * r_ij_z,
        
        float * r2 
        )
{
	const float cutoff2 = 144.0f;
        // distance calc
	/* r_ij is vector from atom i to atom j */
        *r_ij_x = pos_x_j - pos_x_i;
        *r_ij_y = pos_y_j - pos_y_i;
        *r_ij_z = pos_z_j - pos_z_i;

        /* r2 is square of pairwise distance */
        *r2 = *r_ij_x * *r_ij_x + *r_ij_y * *r_ij_y + *r_ij_z * *r_ij_z;

	if ((*r2 < cutoff2) && (*r2 != 0))	// self-referential !
		return 1;

	// else
	return 0;
}


void pe_1(

        unsigned int k,

        float q_i,
        float q_j,

        float r_ij_x,
        float r_ij_y,
        float r_ij_z,
        
        float r2,
        
            float sub_force_x[4],
            float sub_force_y[4],
            float sub_force_z[4]

            //float *force_x_temp,
            //float *force_y_temp,
            //float *force_z_temp
        
        )
{
float u = 0.0, du_r = 0.0;  /* else compiler generates warning */
// constants

const float elec_const = 332.0636000000000080945028457790613174438476562500000000000000000000000000000000000000000000000000000000f;
const float cutoff2 = 144.0f;
const float ewald_coef = 0.2579518376363685305818762572016566991806030273437500000000000000000000000000000000000000000000000000f;
const float ewald_grad_coef = 0.2910674797028824367650656768091721460223197937011718750000000000000000000000000000000000000000000000f;
const float roff2 = 144.000000f;
const float ron2 = 100.000000f;
const float inv_denom_switch = 0.0000117392937640871531507474195832330110533803235739469528198242187500000000000000000000000000000000f;

	// force_calc_elec
	// ####################################################################
	 float r; 
	 float inv_r;   /* 1/r */ 
	 float inv_r2;  /* 1/r^2 */ 
	 float a, b; 

	float f_elec_j_x;
	float f_elec_j_y;
	float f_elec_j_z;
	
	float f_vdw_j_x;
	float f_vdw_j_y;
	float f_vdw_j_z;

	 r = sqrtf(r2); 
	 inv_r = 1.0f / r; 
	 inv_r2 = inv_r * inv_r; 
	 a = r * (ewald_coef); 

	 b = my_erfc(a); 

	 u = elec_const * b * inv_r; 
	 du_r = -(elec_const * (ewald_grad_coef) * my_exp(-a*a) + u) * inv_r2; 

	 u *= q_i * q_j;
	 du_r *= q_i * q_j;
	 f_elec_j_x = du_r * r_ij_x;
	 f_elec_j_y = du_r * r_ij_y;
	 f_elec_j_z = du_r * r_ij_z;
	
	// ####################################################################
	// force_cal_vdw
	// ####################################################################

	// profiling result
	a = 327.917480f;
	b = 10.474518f;

	float inv_r6;   /* 1/r^6 */ 
	float inv_r12;  /* 1/r^12 */ 
	float a_r12;    /* a/r^12 */ 
	float b_r6;     /* b/r^6 */ 
	float w;        /* full vdw potential */ 
	float dw_r;     /* dw/r */ 
	float s;        /* switching function */ 
	float ds_r;     /* ds/r */ 

	inv_r6 = inv_r2 * inv_r2 * inv_r2; 
	inv_r12 = inv_r6 * inv_r6; 
	a_r12 = (a) * inv_r12; 
	b_r6 = (b) * inv_r6; 
	w = a_r12 - b_r6; 
	dw_r = (-12.0f * a_r12 + 6.0f * b_r6) * inv_r2; 
	if (r2 > ron2)
	{ 
		s = (roff2 - r2) * (roff2 - r2) 
		* (roff2 + 2.0f * r2 - 3.0f * ron2) * inv_denom_switch; 
		ds_r = 12.0f * (roff2 - r2) * (ron2 - r2) * inv_denom_switch; 
		u = w * s; 
		du_r = w * ds_r + dw_r * s; 
	} 
	else 
	{ 
	  	u = w; 
	  	du_r = dw_r; 
	} 

	f_vdw_j_x = du_r * r_ij_x;
	f_vdw_j_y = du_r * r_ij_y;
	f_vdw_j_z = du_r * r_ij_z;

	
	float force_x = - (f_elec_j_x + f_vdw_j_x);
	float force_y = - (f_elec_j_y + f_vdw_j_y);
	float force_z = - (f_elec_j_z + f_vdw_j_z);

	//*force_x_temp += force_x;
	//*force_y_temp += force_y;
	//*force_z_temp += force_z;
    sub_force_x[k%4] += force_x;
    sub_force_y[k%4] += force_y;
    sub_force_z[k%4] += force_z;
}


//static int o_cell_cnt[NUM_CELLS] = {0};

void pe_2(
        int index, int j,
        float q_i,
        float q_j,
        
        float pos_x_i,
        float pos_y_i,
        float pos_z_i,

        float vel_x,
        float vel_y,
        float vel_z,
        float mass,

        float force_x_temp,
        float force_y_temp,
        float force_z_temp,

        float* o_cell_pos, float* o_cell_vel)
{

// implementation of function: motion update
#define delta_t 1e-12f   // delta t
#define delta_t2 1e-24f  // delta t ^ 2
        float new_pos_x;
        float new_pos_y;
        float new_pos_z;
        float new_q;
        float new_vel_x;
        float new_vel_y;
        float new_vel_z;
        float new_mass;

	new_mass = mass;
	new_q = q_i;

	// monte-carlo algorithm
	new_pos_x = pos_x_i + (delta_t * vel_x) + (delta_t2 * force_x_temp / 2.0f / mass);
	new_pos_y = pos_y_i + (delta_t * vel_y) + (delta_t2 * force_y_temp / 2.0f / mass);
	new_pos_z = pos_z_i + (delta_t * vel_z) + (delta_t2 * force_z_temp / 2.0f / mass);

	new_vel_x = vel_x + (delta_t * force_x_temp / mass);
	new_vel_y = vel_y + (delta_t * force_y_temp / mass);
	new_vel_z = vel_z + (delta_t * force_z_temp / mass);
	// handle boundary condition
	// pos info :
	//pos_x_max = 54.430199   pos_x_min = -54.430199        => x: -55 ~ +55
	//pos_y_max = 54.430199   pos_y_min = -54.426201        => y: -55 ~ +55
	//pos_z_max = 38.876999   pos_z_min = -38.877998        => z: -40 ~ +40
	if (new_pos_x < -55.00f)   new_pos_x = -55;
	if (new_pos_x > 55.00f)    new_pos_x = 55;
	if (new_pos_y < -55.00f)   new_pos_y = -55;
	if (new_pos_y > 55.00f)    new_pos_y = 55;
	if (new_pos_z < -40.00f)   new_pos_z = -40;
	if (new_pos_z > 40.00f)    new_pos_z = 40;


	// write ...
	o_cell_pos[index*500*4 + j*4 + 0] = new_pos_x;
	o_cell_pos[index*500*4 + j*4 + 1] = new_pos_y;
	o_cell_pos[index*500*4 + j*4 + 2] = new_pos_z;
	o_cell_pos[index*500*4 + j*4 + 3] = new_q;

	o_cell_vel[index*500*4 + j*4 + 0] = new_vel_x;
	o_cell_vel[index*500*4 + j*4 + 1] = new_vel_y;
	o_cell_vel[index*500*4 + j*4 + 2] = new_vel_z;
	o_cell_vel[index*500*4 + j*4 + 3] = new_mass;

}



#pragma ACCEL kernel
void namd_kernel(int cell_to_run, int neighbour_to_run, int cell_size,
                 float* i_cell_pos, float* i_cell_vel, float* o_cell_pos, float* o_cell_vel )
{

    int index;
    // kernel starts here
    for (index = 0; index < cell_to_run; index++)
    {


        // pe_1 temp vars


        int index_x[27]; // = {index/70};
        int index_y[27]; // = { (index/7) % 10 };
        int index_z[27]; // = { index % 7 };

        int iii;
        for (iii = 0; iii < 27; iii++)
        {
//#pragma ACCEL pipeline
            index_x[iii] = index/70;
            index_y[iii] = (index/7) % 10 ;
            index_z[iii] = index % 7 ;
        }

        index_x[1]++;
        index_x[2]--;
        index_y[3]++;
        index_y[4]--;
        index_z[5]++;
        index_z[6]--;

        index_x[7]++; index_y[7]++;
        index_x[8]++; index_y[8]--;
        index_x[9]--; index_y[9]++;
        index_x[10]--; index_y[10]--;

        index_x[11]++; index_z[11]++;
        index_x[12]++; index_z[12]--;
        index_x[13]--; index_z[13]++;
        index_x[14]--; index_z[14]--;

        index_y[15]++; index_z[15]++;
        index_y[16]++; index_z[16]--;
        index_y[17]--; index_z[17]++;
        index_y[18]--; index_z[18]--;

        index_x[19]++; index_y[19]++; index_z[19]++;
        index_x[20]++; index_y[20]++; index_z[20]--;
        index_x[21]++; index_y[21]--; index_z[21]++;
        index_x[22]++; index_y[22]--; index_z[22]--;
        index_x[23]--; index_y[23]++; index_z[23]++;
        index_x[24]--; index_y[24]++; index_z[24]--;
        index_x[25]--; index_y[25]--; index_z[25]++;
        index_x[26]--; index_y[26]--; index_z[26]--;

        int nbr_index[27];

        unsigned int i, j, k;

        float ha_buffer[500*4]; // 4 floats * max number of atoms in each cell
        float ha_vel_buffer[500*4]; // 4 floats * max number of atoms in each cell
        //float na_bram[NA_BRAM_SIZE];
        float na_bram[512*4];
        float na_bram_p[NA_BRAM_SIZE];

        memcpy (ha_buffer, i_cell_pos + (index*500*4) , i_cell_cnt(index, cell_size)*4*sizeof(float));
        memcpy (ha_vel_buffer, i_cell_vel + (index*500*4) , i_cell_cnt(index, cell_size)*4*sizeof(float));

        for (i = 0; i < 27; i++)
        {
//#pragma ACCEL pipeline
            nbr_index[i] = index_z[i] + index_y[i]*7 + index_x[i]*70;
        }


        int na_counter = 0;	// counter for neighbor atoms 
        int ll, mm;


        unsigned int nbr_atom_cnt[27];

        // memcpy into na_bram
        for (i = 0; i < 27; i++)
        {
            // handle boundary 
            if ( (index_x[i] >= 0) && (index_x[i] < DIM_X)
                &&  (index_y[i] >= 0) && (index_y[i] < DIM_Y)
                &&  (index_z[i] >= 0) && (index_z[i] < DIM_Z)
                )
            {
                unsigned int atom_cnt = i_cell_cnt(nbr_index[i],cell_size);

                //memcpy (na_bram + na_counter*4, i_cell_pos + (nbr_index[i]*500*4), i_cell_cnt(nbr_index[i])*4*sizeof(float));
                memcpy (na_bram, i_cell_pos + (nbr_index[i]*500*4), atom_cnt *4*sizeof(float));

                for (mm = 0; mm < atom_cnt; mm++) 
                {
//#pragma ACCEL pipeline
                    na_bram_p[512*4*i + mm*4 + 0] = na_bram[mm*4+0];
                    na_bram_p[512*4*i + mm*4 + 1] = na_bram[mm*4+1];
                    na_bram_p[512*4*i + mm*4 + 2] = na_bram[mm*4+2];
                    na_bram_p[512*4*i + mm*4 + 3] = na_bram[mm*4+3];
                }

                na_counter += atom_cnt;
                nbr_atom_cnt[i] = atom_cnt;
            }
            else
                nbr_atom_cnt[i] = 0;
        }

        //na_bram[NA_BRAM_SIZE-1] = na_counter;




        // 	for all home atoms
        //for (j = 0; j < i_cell_cnt(index); j++)
        for (j = 0; j < nbr_atom_cnt[0]; j++)
        {

            float pos_x_i;
            float pos_y_i;
            float pos_z_i;
            float pos_x_j;
            float pos_y_j;
            float pos_z_j;

            float q_i;
            float q_j;

            pos_x_i = ha_buffer[j*4+0]; //i_cell_pos[index*500*4 + j*4 + 0];
            pos_y_i = ha_buffer[j*4+1]; //i_cell_pos[index*500*4 + j*4 + 1];
            pos_z_i = ha_buffer[j*4+2]; //i_cell_pos[index*500*4 + j*4 + 2];
            q_i     = ha_buffer[j*4+3]; //i_cell_pos[index*500*4 + j*4 + 3];

            float force_x_temp;
            float force_y_temp;
            float force_z_temp;

            force_x_temp = 0;
            force_y_temp = 0;
            force_z_temp = 0;

            float sub_force_x[4] = { 0.0, 0.0, 0.0, 0.0 } ;
            float sub_force_y[4] = { 0.0, 0.0, 0.0, 0.0 } ;
            float sub_force_z[4] = { 0.0, 0.0, 0.0, 0.0 } ;

            // 		for all neighbor cells
            for (i = 0; i < neighbour_to_run; i++)	// for large version
            {
                // handle boundary
                if ( (index_x[i] >= 0) && (index_x[i] < DIM_X)
                    &&  (index_y[i] >= 0) && (index_y[i] < DIM_Y)
                    &&  (index_z[i] >= 0) && (index_z[i] < DIM_Z)
                    )
                {
                    // 				for all neighbor atoms
                    unsigned int nb_cnt = nbr_atom_cnt[i]; //i_cell_cnt(nbr_index[i]);
                    for (k = 0; k < nb_cnt; k++)
                    {
//#pragma AP pipeline II=1
//#pragma ACCEL pipeline
                        //cycle_cnt++;
#pragma AP dependence  variable=sub_force_x array inter false
#pragma AP dependence  variable=sub_force_y array inter false
#pragma AP dependence  variable=sub_force_z array inter false
//#pragma AP array_partition variable=na_bram_p cyclic factor=4
                        float r_ij_x;
                        float r_ij_y;
                        float r_ij_z;

                        pos_x_j = na_bram_p[512*4*i + k*4 + 0];
                        pos_y_j = na_bram_p[512*4*i + k*4 + 1];
                        pos_z_j = na_bram_p[512*4*i + k*4 + 2];
                        q_j     = na_bram_p[512*4*i + k*4 + 3];
                        //					if within cutoff(pe_0)

                        float r2;
                        if (pe_0(   pos_x_i, pos_y_i, pos_z_i, 
                            pos_x_j, pos_y_j, pos_z_j, 
                            &r_ij_x, &r_ij_y, &r_ij_z,
                            &r2) == 1)
                        {
                            pe_1(   k, q_i, q_j,

                                r_ij_x, r_ij_y, r_ij_z,

                                r2,

                                sub_force_x ,
                                sub_force_y ,
                                sub_force_z 

                                );
                        }
                    }
                } // end if
            } // end for loop: i
            //		motion_update (pe_2)

            for (ll = 0; ll < 4; ll++) {
                force_x_temp += sub_force_x[ll];
                force_y_temp += sub_force_y[ll];
                force_z_temp += sub_force_z[ll];
            }



            {
                float vel_x;
                float vel_y;
                float vel_z;
                float mass;

                vel_x   = ha_vel_buffer[j*4+0]; //i_cell_vel[index*500*4 + j*4 + 0];
                vel_y   = ha_vel_buffer[j*4+1]; //i_cell_vel[index*500*4 + j*4 + 1];
                vel_z   = ha_vel_buffer[j*4+2]; //i_cell_vel[index*500*4 + j*4 + 2];
                mass    = ha_vel_buffer[j*4+3]; //i_cell_vel[index*500*4 + j*4 + 3];
                pe_2(
                    index, j,
                    q_i, q_j,
                    pos_x_i, pos_y_i, pos_z_i,
                    vel_x, vel_y, vel_z,
                    mass,
                    force_x_temp ,
                    force_y_temp ,
                    force_z_temp ,
                    o_cell_pos, o_cell_vel);
            }
        } // end for loop: j
    } // end for loop: index

}


