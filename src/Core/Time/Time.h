#pragma once

extern float time_scale;

void time_init();
void time_update_delta();
float time_delta();
float time_delta_unscaled();
float time_elapsed();