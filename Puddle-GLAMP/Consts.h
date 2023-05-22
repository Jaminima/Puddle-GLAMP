#pragma once

//#define EnableReducedTracing

#ifndef __Consts
const unsigned int w = 1024, h = 1024;
//const unsigned int w = 512, h = 512;
//const unsigned int w = 256, h = 256;
//const unsigned int w = 128, h = 128;

const unsigned int w_half = w / 2, h_half = h / 2;

const unsigned int maxView = 100;

const float camMoveStep = 0.5f;
const float camSensitvity = 0.001f;
#endif

#define __Consts
