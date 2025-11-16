// spawn.h
// Created by Fred Nora.

#ifndef __DISP_SPAWN_H
#define __DISP_SPAWN_H    1


// The spawn routine need to make a eoi.
void spawn_set_eoi_state(void);
// The spawn routine do not need to make a eoi.
void spawn_reset_eoi_state(void);

void psSpawnThreadByTID(tid_t tid);


#endif 