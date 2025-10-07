/**
 * Simple Shooter Game for NW (SSGN)

 * Copyright (C) 2022 JeFaisDesSpaghettis
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 * See gpl-3.0.md for more information
 */

#include <cstdint>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../../api/extapp_api.h"

#include "GRANATA.h"
#include "patata16.h"
#include "poulpi32.h"

extern "C" void extapp_main();

typedef struct _entity
{
    bool exists;
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    const uint8_t* sprite;
    const uint8_t* eraser;
}entity;

const uint8_t ENEMY_ERASER[16*16*2] = {0};
const uint8_t PLAYER_ERASER[32*32*2] = {0};

const char* LICENSE_INFO =
"Simple Shooter Game for NW (SSGN) v1.2.0\n"
    "\tLicensed under GPL-3.0,\n"
    "\t(see https://www.gnu.org/licenses/)\n"
"https://github.com/JeFaisDesSpaghettis/SSGN-C\n"
"\nWAIT 3 SECONDS AND PRESS ANY KEY TO START";

// Fire 5 times, 3 grenades salvos
const size_t GRENADE_COUNT = 18;
entity GRENADES[GRENADE_COUNT];

// 20 enemies
const size_t ENEMY_COUNT = 21;
entity ENEMIES[ENEMY_COUNT];

entity PLAYER;
uint32_t LEHMER_SEED = 0;

void wait_for_key_pressed()
{
	while (!extapp_scanKeyboard())
		extapp_msleep(10);
}

void wait_for_key_released()
{
	while (extapp_scanKeyboard())
		extapp_msleep(10);
}

void clear_screen()
{
    extapp_pushRectUniform(0, 0, 320, 240, 0x0);
}

void init_entity(
    entity* entity_,
    bool exists,
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    const uint8_t* sprite,
    const uint8_t* eraser)
{
    entity_->exists = exists;
    entity_->x = x;
    entity_->y = y;
    entity_->w = w;
    entity_->h = h;
    entity_->sprite = sprite;
    entity_->eraser = eraser;
}

uint32_t rand_lehmer()
{
	LEHMER_SEED += 0xe120fc15;
	uint64_t tmp;
	tmp = (uint64_t)LEHMER_SEED * 0x4a39b70d;
	uint32_t m1 = (tmp >> 32) ^ tmp;
	tmp = (uint64_t)m1 * 0x12fad5c9;
	uint32_t m2 = (tmp >> 32) ^ tmp;
	return m2;
}

bool is_colliding(entity e1, entity e2)
{
    return !(((e1.x + e1.w) < e2.x) || ((e2.x + e2.w) < e1.x) || ((e1.y + e1.h) < e2.y) || ((e2.y + e2.h) < e1.y));
}

void extapp_main(void)
{
    clear_screen();
    extapp_drawTextSmall(LICENSE_INFO, 0, 0, 0xFFFF, 0x0, false);
    extapp_msleep(3000);
    wait_for_key_pressed();
    wait_for_key_released();
    clear_screen();

    uint64_t kb = 0;
    char hud_buffer[64] = {0};

    int cooldown = 0;
    int cooldown_max = 8;
    int cooldown_rate = 1;
    int regen_cooldown = 0;
    int regen_cooldown_max = 24 * 5;
    int regen_cooldown_rate = 1;
    int spawn_cooldown = 0;
    int spawn_cooldown_max = 16;
    int spawn_cooldown_rate = 1;

    size_t active_grenades = 0;
    size_t active_enemies = 0;
    int enemy_speed = 2;
    int health = 10;
    int score = 0;

    // Init globals
        // Init sprites
        for (size_t i = 0; i < GRENADE_COUNT; i++)
            init_entity(&GRENADES[i], false, 0, -1, 4, 12, (uint8_t*)GRANATA, NULL);
        for (size_t i = 0; i < ENEMY_COUNT; i++)
            init_entity(&ENEMIES[i], false, 0, 0, 16, 16, (uint8_t*)patata16, ENEMY_ERASER);
        init_entity(
            &PLAYER, true,
            LCD_WIDTH - (32/2), LCD_HEIGHT - (32/2),
            32, 32,
            (uint8_t*)poulpi32, PLAYER_ERASER);
    // END Init

    while (kb != SCANCODE_Back && health > 0)
    {
        uint64_t start = extapp_millis();

        // Render I
        extapp_waitForVBlank();
            // Erase
            for (size_t i = 0; i < GRENADE_COUNT; i++)
            {
                if (GRENADES[i].exists)
                    extapp_pushRect(GRENADES[i].x, GRENADES[i].y, GRENADES[i].w, GRENADES[i].h, (uint16_t*)(GRENADES[i].eraser));
            }
            for (size_t i = 0; i < ENEMY_COUNT; i++)
            {
                if (ENEMIES[i].exists)
                    extapp_pushRect(ENEMIES[i].x, ENEMIES[i].y, ENEMIES[i].w, ENEMIES[i].h, (uint16_t*)(ENEMIES[i].eraser));
            }
            extapp_pushRect(PLAYER.x, PLAYER.y, PLAYER.w, PLAYER.h, (uint16_t*)(PLAYER.eraser));
        // End Render I

        // Logic
            // Cooldowns
            cooldown += cooldown_rate;
            regen_cooldown += regen_cooldown_rate;
            spawn_cooldown += spawn_cooldown_rate;

            // Input
            kb = extapp_scanKeyboard();
            if (kb & SCANCODE_Left)
                PLAYER.x -= 8;
            if (kb & SCANCODE_Right)
                PLAYER.x += 8;
            if (kb & SCANCODE_Up)
                PLAYER.y -= 8;
            if (kb & SCANCODE_Down)
                PLAYER.y += 8;

            if (kb & SCANCODE_OK)
            {
                if (cooldown >= cooldown_max)
                {
                    GRENADES[active_grenades + 0].exists = true;
                    GRENADES[active_grenades + 0].x = PLAYER.x - (GRENADES[active_grenades + 0].w / 2) - 0;
                    GRENADES[active_grenades + 0].y = PLAYER.y - GRENADES[active_grenades + 0].h - 2;
                    GRENADES[active_grenades + 1].exists = true;
                    GRENADES[active_grenades + 1].x = PLAYER.x - (GRENADES[active_grenades + 0].w / 2) + (PLAYER.w / 2);
                    GRENADES[active_grenades + 1].y = PLAYER.y - GRENADES[active_grenades + 0].h - 2;
                    GRENADES[active_grenades + 2].exists = true;
                    GRENADES[active_grenades + 2].x = PLAYER.x - (GRENADES[active_grenades + 0].w / 2) + PLAYER.w;
                    GRENADES[active_grenades + 2].y = PLAYER.y - GRENADES[active_grenades + 0].h - 2;
                    active_grenades %= GRENADE_COUNT;
                    active_grenades += 3;
                    cooldown = 0;
                }
            }
            if (kb & SCANCODE_Shift)
            {
                if (regen_cooldown >= regen_cooldown_max)
                {
                    if (score > 50)
                    {
                        score -= 50;
                        health += 2;
                    }
                    regen_cooldown = 0;
                }
            }

            // Player collision
            if (PLAYER.x < 0)
                PLAYER.x = 0;
            else if (LCD_WIDTH < (PLAYER.x + PLAYER.w))
                PLAYER.x = LCD_WIDTH - PLAYER.w;
            if (PLAYER.y < 0)
                PLAYER.y = 0;
            else if (LCD_HEIGHT < (PLAYER.y + PLAYER.h))
                PLAYER.y = LCD_HEIGHT - PLAYER.h;

            // Spawn enemies
            if (spawn_cooldown >= spawn_cooldown_max)
            {
                if (!ENEMIES[active_enemies].exists)
                {
                    ENEMIES[active_enemies].exists = true;
                    ENEMIES[active_enemies].x = rand_lehmer() % (LCD_WIDTH - ENEMIES->w);
                    ENEMIES[active_enemies].y = 0;
                    active_enemies++;
                    active_enemies %= ENEMY_COUNT;
                    spawn_cooldown = 0;
                }
            }

            // Discard out of the screen grenades
            for (size_t i = 0; i < GRENADE_COUNT; i++)
            {
                GRENADES[i].y -= 5;
                if (GRENADES[i].y < -GRENADES[i].h)
                    GRENADES[i].exists = false;
            }

            // Check enemies
            for (size_t i = 0; i < ENEMY_COUNT; i++)
            {
                if (ENEMIES[i].exists)
                {
                    enemy_speed = (score + 50) / 50;
                    if (enemy_speed > 3)
                        enemy_speed = 3;
                    spawn_cooldown_max = 16 - (score / 50);
                    ENEMIES[i].y += enemy_speed;
                    if (LCD_HEIGHT < (ENEMIES[i].y + ENEMIES[i].h))
                    {
                        ENEMIES[i].exists = false;
                        health -= 1;
                    }
                    for (size_t j = 0; j < GRENADE_COUNT; j++)
                    {
                        if (GRENADES[j].exists && is_colliding(ENEMIES[i], GRENADES[j]))
                        {
                            ENEMIES[i].exists = false;
                            score += 1;
                        }
                    }
                }
            }

        // Render II
            // Entities
            for (size_t i = 0; i < GRENADE_COUNT; i++)
            {
                if (GRENADES[i].exists)
                    extapp_pushRect(GRENADES[i].x, GRENADES[i].y, GRENADES[i].w, GRENADES[i].h, (uint16_t*)(GRENADES[i].sprite));
            }
            for (size_t i = 0; i < ENEMY_COUNT; i++)
            {
                if (ENEMIES[i].exists)
                    extapp_pushRect(ENEMIES[i].x, ENEMIES[i].y, ENEMIES[i].w, ENEMIES[i].h, (uint16_t*)(ENEMIES[i].sprite));
            }
            extapp_pushRect(PLAYER.x, PLAYER.y, PLAYER.w, PLAYER.h, (uint16_t*)(PLAYER.sprite));

            // HUD
            sprintf(hud_buffer, "TIMING: %llu (ms) | HEALTH %03i | SCORE %03i", (extapp_millis() - start), health, score);
            extapp_drawTextSmall(hud_buffer, 0, 0, 0xFFFF, 0x0, false);
        // End render II

        // Sleep
        while ((extapp_millis() - start) < 16);
    }

 
    wait_for_key_pressed();
    wait_for_key_released();
    return;
}
