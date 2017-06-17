#pragma once
#include <allegro5\allegro.h>
#include <allegro5\allegro_primitives.h>
#include <allegro5\allegro_font.h>

int width = 800;
int height = 300;

//if (wav_header.NumOfChan == 2) {
//	for (uint32_t i = 0; i < data_chunk.size; i += 4) {
//		short left = *(short *)&data[i];
//		short right = *(short *)&data[i + 2];
//		short monoSample = (int(left) + right) / 2;
//		double t = monoSample / 32768.0;
//		d.push_back(t);
//	}
//}
//else {
//	for (uint32_t i = 0; i < data_chunk.size; i++) {
//		short c = *(short *)&data[i];
//		double t = c / 32768.0;
//		d.push_back(t);
//	}
//}

void Draw(char* data, uint32_t size, bool stereo) {

	al_init();
	al_init_primitives_addon();
	al_install_keyboard();

	ALLEGRO_KEYBOARD_STATE klawiatura;
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	ALLEGRO_DISPLAY *okno = al_create_display(width, height);
	al_set_window_title(okno, "Drawer Window");
	ALLEGRO_EVENT_QUEUE *kolejka = al_create_event_queue(); // event_queue
	al_register_event_source(kolejka, al_get_display_event_source(okno));
	ALLEGRO_EVENT zdarzenie; // event

	bool exit = false;
	while (!exit) {
		al_get_next_event(kolejka, &zdarzenie);
		if (zdarzenie.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;

		al_get_keyboard_state(&klawiatura);
		al_clear_to_color(al_map_rgb(150, 150, 150));

		if (stereo) {
			if (size > 0) {
				for (int i = 0; i < width; i++) {
					short cl = *(short *)&data[(i*size) / width];
					double sl = cl / 32768.0;
					int yl = (height / 2) * sl;
					al_draw_line(i, (height / 4) - yl, i, (height / 4) + yl, al_map_rgb(0, 255, 0), 1);

					short cr = *(short *)&data[(i*size) / width + 2];
					double sr = cr / 32768.0;
					int yr = (height / 2) * sr;
					al_draw_line(i, (height - (height / 4)) - yr, i, (height - (height / 4)) + yr, al_map_rgb(0, 255, 0), 1);
				}
			}
			al_draw_line(0, (height / 4), width, (height / 4), al_map_rgb(0, 0, 255), 1);
			al_draw_line(0, (height - (height / 4)), width, (height - (height / 4)), al_map_rgb(0, 0, 255), 1);
			al_draw_line(0, (height / 2), width, (height / 2), al_map_rgb(255, 0, 0), 1);
		}
		else {
			if (size > 0) {
				for (int i = 0; i < width; i++) {
					short c = *(short *)&data[(i*size) / width];
					double s = c / 32768.0;
					int y = (height / 2) * s;
					al_draw_line(i, (height / 2) - y, i, (height / 2) + y, al_map_rgb(0, 255, 0), 1);
				}
			}
			al_draw_line(0, (height / 2), width, (height / 2), al_map_rgb(0, 0, 255), 1);
		}

		al_flip_display();
		al_rest(0.005);
	}

}

void Draw(int16_t* data, uint32_t size, bool stereo) {

	al_init();
	al_init_primitives_addon();
	al_install_keyboard();

	ALLEGRO_KEYBOARD_STATE klawiatura;
	al_set_new_display_flags(ALLEGRO_WINDOWED);
	ALLEGRO_DISPLAY *okno = al_create_display(width, height);
	al_set_window_title(okno, "Drawer Window");
	ALLEGRO_EVENT_QUEUE *kolejka = al_create_event_queue(); // event_queue
	al_register_event_source(kolejka, al_get_display_event_source(okno));
	ALLEGRO_EVENT zdarzenie; // event

	bool exit = false;
	while (!exit) {
		al_get_next_event(kolejka, &zdarzenie);
		if (zdarzenie.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			break;

		al_get_keyboard_state(&klawiatura);
		al_clear_to_color(al_map_rgb(150, 150, 150));

		if (stereo) {
			if (size > 0) {
				for (int i = 0; i < width; i++) {
					short cl = *(short *)&data[(i*size) / width];
					double sl = cl / 32768.0;
					int yl = (height / 2) * sl;
					al_draw_line(i, (height / 4) - yl, i, (height / 4) + yl, al_map_rgb(0, 255, 0), 1);

					short cr = *(short *)&data[(i*size) / width + 2];
					double sr = cr / 32768.0;
					int yr = (height / 2) * sr;
					al_draw_line(i, (height - (height / 4)) - yr, i, (height - (height / 4)) + yr, al_map_rgb(0, 255, 0), 1);
				}
			}
			al_draw_line(0, (height / 4), width, (height / 4), al_map_rgb(0, 0, 255), 1);
			al_draw_line(0, (height - (height / 4)), width, (height - (height / 4)), al_map_rgb(0, 0, 255), 1);
			al_draw_line(0, (height / 2), width, (height / 2), al_map_rgb(255, 0, 0), 1);
		}
		else {
			if (size > 0) {
				for (int i = 0; i < width; i++) {
					short c = *(short *)&data[(i*size) / width];
					double s = c / 32768.0;
					int y = (height / 2) * s;
					al_draw_line(i, (height / 2) - y, i, (height / 2) + y, al_map_rgb(0, 255, 0), 1);
				}
			}
			al_draw_line(0, (height / 2), width, (height / 2), al_map_rgb(0, 0, 255), 1);
		}

		al_flip_display();
		al_rest(0.5);
	}

}