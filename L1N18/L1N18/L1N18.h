#pragma once

void send_to_all(int procnum, int myrank, int  message[5], const int& m, int TAG);

void is_intersects(bool& is_ident, int  message[5], int  buffer[5]);

void print_array(int  message[5]);
