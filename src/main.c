#include <pebble.h>

#define KEY_BACKGROUND_COLOR 0
#define KEY_TICK_COLOR 1
#define KEY_HOUR_COLOR 2
#define KEY_MINUTE_COLOR 3
#define KEY_DOT_COLOR 4
#define KEY_DATE_COLOR 5
#define KEY_BLUETOOTH_VIBES 6

#if defined(PBL_RECT)
  #define ROUND false
  #define TICK_LINE_LENGTH 111 //px
#elif defined(PBL_ROUND)
  #define ROUND true 
  #define TICK_LINE_LENGTH 90 //px
#endif

#define DATE_LAYER_VERTICAL_OFFSET 20 //px    //to vertically center the left and right date layers
#define DATE_LAYER_HORIZONTAL_OFFSET 60       //to move the left and right date layers inward
#define DATE_LAYER_BOTTOM_OFFSET 40           //height of the bottom date layer

#define TICK_LENGTH 8 //px    //length of the outside ticks
#define TICK_WIDTH 2 //px     //width  of the outside ticks
#define CENTER_RADIUS 3 //px  //radius of the center circle

#define HAND_WIDTH 7  //px
#define STEM_LENGTH 0.3  //as percentage of hand length
#define HOLE_WIDTH 2  //px(hole in hands)
#define HOLE_START 0.4  //as percentage of hand length (zero is center)
#define HOLE_END 0.8    //as percentage of hand length (zero is center)

static Window *watch_window;
static Layer *hands_layer, *background_layer;
static TextLayer *date_layer_right, *date_layer_left, *date_layer_bottom;

//config variables
static GColor background_color, tick_color, hour_color,               
              minute_color, dot_color, date_color;                    
static bool bluetooth_vibes_bool;                                     


static void hands_update_proc(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  GPoint center = GPoint(bounds.size.w/2, bounds.size.h/2);
  
  int16_t minute_hand_length = bounds.size.w/2 * 0.9;
  int16_t hour_hand_length = minute_hand_length * 0.75;
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  int32_t minute_angle = TRIG_MAX_ANGLE * (t->tm_min / 60.0); 
  int32_t hour_angle = TRIG_MAX_ANGLE * ((t->tm_hour + (t->tm_min / 60.0))/ 12.0); 
  
  GPoint minute_point, minute_stem_point, minute_hole_start_point, minute_hole_end_point;
  GPoint hour_point, hour_stem_point, hour_hole_start_point, hour_hole_end_point;
  //minute/hour point - the tip of the minute/hour hand
  //stem point - where the stem ends (line from center to stem point is the stem)
  //hole start point - where the hole starts    (line from hole start point to
  //hole end point - where the hole ends         hole end point is the hole)
  
  //slugging through the math
  minute_point.y = ((int32_t)(-cos_lookup(minute_angle)) * (int16_t)(minute_hand_length) / TRIG_MAX_RATIO) + center.y;
  minute_point.x = ((int32_t)( sin_lookup(minute_angle)) * (int16_t)(minute_hand_length) / TRIG_MAX_RATIO) + center.x;
  minute_stem_point.y = ((int32_t)(-cos_lookup(minute_angle)) * 
                         (int16_t)(minute_hand_length * STEM_LENGTH) / TRIG_MAX_RATIO) + center.y;
  minute_stem_point.x = ((int32_t)( sin_lookup(minute_angle)) * 
                         (int16_t)(minute_hand_length * STEM_LENGTH) / TRIG_MAX_RATIO) + center.x;
  
  minute_hole_start_point.y = ((int32_t)(-cos_lookup(minute_angle)) * 
                               (int16_t)(minute_hand_length * HOLE_START) / TRIG_MAX_RATIO) + center.y;
  minute_hole_start_point.x = ((int32_t)( sin_lookup(minute_angle)) * 
                               (int16_t)(minute_hand_length * HOLE_START) / TRIG_MAX_RATIO) + center.x;
  minute_hole_end_point.y = ((int32_t)(-cos_lookup(minute_angle)) * 
                             (int16_t)(minute_hand_length * HOLE_END) / TRIG_MAX_RATIO) + center.y;
  minute_hole_end_point.x = ((int32_t)( sin_lookup(minute_angle)) * 
                             (int16_t)(minute_hand_length * HOLE_END) / TRIG_MAX_RATIO) + center.x;
  
  hour_point.y = ((int32_t)(-cos_lookup(hour_angle)) * (int16_t)(hour_hand_length) / TRIG_MAX_RATIO) + center.y;
  hour_point.x = ((int32_t)( sin_lookup(hour_angle)) * (int16_t)(hour_hand_length) / TRIG_MAX_RATIO) + center.x;
  hour_stem_point.y = ((int32_t)(-cos_lookup(hour_angle)) * (int16_t)(hour_hand_length * STEM_LENGTH) / TRIG_MAX_RATIO) + center.y;
  hour_stem_point.x = ((int32_t)( sin_lookup(hour_angle)) * (int16_t)(hour_hand_length * STEM_LENGTH) / TRIG_MAX_RATIO) + center.x;
  
  hour_hole_start_point.y = ((int32_t)(-cos_lookup(hour_angle)) * (int16_t)(hour_hand_length * HOLE_START) / TRIG_MAX_RATIO) + center.y;
  hour_hole_start_point.x = ((int32_t)( sin_lookup(hour_angle)) * (int16_t)(hour_hand_length * HOLE_START) / TRIG_MAX_RATIO) + center.x;
  hour_hole_end_point.y = ((int32_t)(-cos_lookup(hour_angle)) * (int16_t)(hour_hand_length * HOLE_END) / TRIG_MAX_RATIO) + center.y;
  hour_hole_end_point.x = ((int32_t)( sin_lookup(hour_angle)) * (int16_t)(hour_hand_length * HOLE_END) / TRIG_MAX_RATIO) + center.x;
  
  //minute_hand
  
  graphics_context_set_stroke_color(ctx, minute_color);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, center, minute_stem_point);
  
  graphics_context_set_stroke_width(ctx, HAND_WIDTH);
  graphics_draw_line(ctx, minute_stem_point, minute_point);
  
  graphics_context_set_stroke_color(ctx, background_color);
  graphics_context_set_stroke_width(ctx, HOLE_WIDTH);
  graphics_draw_line(ctx, minute_hole_start_point, minute_hole_end_point);
  
  //hour hand
   
  graphics_context_set_stroke_color(ctx, hour_color);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, center, hour_stem_point);
  
  graphics_context_set_stroke_width(ctx, HAND_WIDTH);
  graphics_draw_line(ctx, hour_stem_point, hour_point);
  
  graphics_context_set_stroke_color(ctx, background_color);
  graphics_context_set_stroke_width(ctx, HOLE_WIDTH);
  graphics_draw_line(ctx, hour_hole_start_point, hour_hole_end_point);
   
  //center dot
  
  graphics_context_set_fill_color(ctx, background_color);
  graphics_fill_circle(ctx, center, CENTER_RADIUS);
  graphics_context_set_stroke_color(ctx, dot_color);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_circle(ctx, center, CENTER_RADIUS);
  
}

static void background_update_proc(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  GPoint center = GPoint(bounds.size.w/2, bounds.size.h/2);
  
  //sort of a dummy
  GPoint tick_point;
  
  graphics_context_set_stroke_color(ctx, tick_color);
  graphics_context_set_stroke_width(ctx, TICK_WIDTH);
  
  //makes a "spider web" shape
  for(int i = 0; i < 12; i++){
    int32_t angle = TRIG_MAX_ANGLE * (i / 12.0);
    tick_point.x = ((int32_t)( sin_lookup(angle)) * (int16_t)(TICK_LINE_LENGTH) / TRIG_MAX_RATIO) + center.x;
    tick_point.y = ((int32_t)(-cos_lookup(angle)) * (int16_t)(TICK_LINE_LENGTH) / TRIG_MAX_RATIO) + center.y;
    graphics_draw_line(ctx, center, tick_point);
  }
  
  //fills in the center, so only the ticks remain
  graphics_context_set_fill_color(ctx, background_color);
  if(ROUND)
    graphics_fill_circle(ctx, center, (bounds.size.w/2 - TICK_LENGTH));
  else
    graphics_fill_rect(ctx, GRect(
      TICK_LENGTH,
      TICK_LENGTH,
      bounds.size.w - (2 * TICK_LENGTH),
      bounds.size.h - (2 * TICK_LENGTH)),
      0, GCornerNone);
}

static void update_date(void){
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  //corrects for 24h time
  int hour = (t->tm_hour < 13) ? t->tm_hour : t->tm_hour - 12;
  
  static char date_buffer[16];
  
  //strategically put the date where the hands are not covering
  if((t->tm_min < 7 || t->tm_min > 23) && (hour < 1 || hour >= 5)){
    strftime(date_buffer, sizeof(date_buffer), "%a\n%d", t);
    text_layer_set_text(date_layer_right, date_buffer);
    text_layer_set_text(date_layer_left, "\0");
    text_layer_set_text(date_layer_bottom, "\0");
  }
  else if((t->tm_min < 37 || t->tm_min > 53) && (hour < 7 || hour > 11)){
    strftime(date_buffer, sizeof(date_buffer), "%a\n%d", t);
    text_layer_set_text(date_layer_left, date_buffer);
    text_layer_set_text(date_layer_right, "\0");
    text_layer_set_text(date_layer_bottom, "\0");
  }
  else{
    strftime(date_buffer, sizeof(date_buffer), "%a %d", t);
    text_layer_set_text(date_layer_bottom, date_buffer);
    text_layer_set_text(date_layer_right, "\0");
    text_layer_set_text(date_layer_left, "\0");
  }
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
  layer_mark_dirty(hands_layer);
  update_date();
}

static void bluetooth_callback(bool connected){
  if((!connected) && (bluetooth_vibes_bool)) vibes_double_pulse();
}

static void inbox_received_handler(DictionaryIterator *iter, void *context){
  //pulls data from config page
  
  Tuple *background_color_t = dict_find(iter, KEY_BACKGROUND_COLOR);
  Tuple *tick_color_t = dict_find(iter, KEY_TICK_COLOR);
  Tuple *hour_color_t = dict_find(iter, KEY_HOUR_COLOR);
  Tuple *minute_color_t = dict_find(iter, KEY_MINUTE_COLOR);
  Tuple *dot_color_t = dict_find(iter, KEY_DOT_COLOR);
  Tuple *date_color_t = dict_find(iter, KEY_DATE_COLOR);
  Tuple *bluetooth_vibes_t = dict_find(iter, KEY_BLUETOOTH_VIBES);
  
  if(background_color_t){
    int background_color_HEX = background_color_t->value->int32;
    persist_write_int(KEY_BACKGROUND_COLOR, background_color_HEX);
    background_color = GColorFromHEX(background_color_HEX);
    window_set_background_color(watch_window, background_color);
    layer_mark_dirty(hands_layer);
    layer_mark_dirty(background_layer);
  }
  if(tick_color_t){
    int tick_color_HEX = tick_color_t->value->int32;
    persist_write_int(KEY_TICK_COLOR, tick_color_HEX);
    tick_color = GColorFromHEX(tick_color_HEX);
    layer_mark_dirty(background_layer);
  }
  if(hour_color_t){
    int hour_color_HEX = hour_color_t->value->int32;
    persist_write_int(KEY_HOUR_COLOR, hour_color_HEX);
    hour_color = GColorFromHEX(hour_color_HEX);
    layer_mark_dirty(hands_layer);
  }
  if(minute_color_t){
    int minute_color_HEX = minute_color_t->value->int32;
    persist_write_int(KEY_MINUTE_COLOR, minute_color_HEX);
    minute_color = GColorFromHEX(minute_color_HEX);
    layer_mark_dirty(hands_layer);
  }
  if(dot_color_t){
    int dot_color_HEX = dot_color_t->value->int32;
    persist_write_int(KEY_DOT_COLOR, dot_color_HEX);
    dot_color = GColorFromHEX(dot_color_HEX);
    layer_mark_dirty(hands_layer);
  }
  if(date_color_t){
    int date_color_HEX = date_color_t->value->int32;
    persist_write_int(KEY_DATE_COLOR, date_color_HEX);
    date_color = GColorFromHEX(date_color_HEX);
    text_layer_set_text_color(date_layer_right, date_color);
    text_layer_set_text_color(date_layer_left, date_color);
    text_layer_set_text_color(date_layer_bottom, date_color);
  }
  if(bluetooth_vibes_t){
    bluetooth_vibes_bool = bluetooth_vibes_t->value->int8;
    persist_write_int(KEY_BLUETOOTH_VIBES, bluetooth_vibes_bool);
  }
  
}

static void init(void){
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  connection_service_subscribe((ConnectionHandlers){
    .pebble_app_connection_handler = bluetooth_callback
  });
  
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(128, 0);
  
  //pulls data from local storage
  if(persist_exists(KEY_BACKGROUND_COLOR))
    background_color = GColorFromHEX(persist_read_int(KEY_BACKGROUND_COLOR));
  else
    background_color = GColorBlack;
  
  if(persist_exists(KEY_TICK_COLOR))
    tick_color = GColorFromHEX(persist_read_int(KEY_TICK_COLOR));
  else 
    tick_color = GColorLightGray;
  
  if(persist_exists(KEY_HOUR_COLOR))
    hour_color = GColorFromHEX(persist_read_int(KEY_HOUR_COLOR));
  else
    hour_color = GColorWhite;
    
  if(persist_exists(KEY_MINUTE_COLOR))
    minute_color = GColorFromHEX(persist_read_int(KEY_MINUTE_COLOR));
  else
    minute_color = GColorWhite;
  
  if(persist_exists(KEY_DOT_COLOR))
    dot_color = GColorFromHEX(persist_read_int(KEY_DOT_COLOR));
  else
    dot_color = GColorWhite;
  
  if(persist_exists(KEY_DATE_COLOR))
    date_color = GColorFromHEX(persist_read_int(KEY_DATE_COLOR));
  else
    date_color = GColorWhite;
  
  bluetooth_vibes_bool = (persist_exists(KEY_BLUETOOTH_VIBES)) ? persist_read_bool(KEY_BLUETOOTH_VIBES) : true;
  
  watch_window = window_create();
  Layer *window_layer = window_get_root_layer(watch_window);
  GRect bounds = layer_get_bounds(window_layer);
  
  window_set_background_color(watch_window, background_color);
  
  background_layer = layer_create(bounds);
  layer_set_update_proc(background_layer, background_update_proc);
  
  hands_layer = layer_create(bounds);
  layer_set_update_proc(hands_layer, hands_update_proc);
  
  date_layer_right = text_layer_create(GRect(
    (bounds.size.w - DATE_LAYER_HORIZONTAL_OFFSET),
    (bounds.size.h/2 - DATE_LAYER_VERTICAL_OFFSET), 
    DATE_LAYER_HORIZONTAL_OFFSET,
    60));
   
  date_layer_left = text_layer_create(GRect(
    0,
    (bounds.size.h/2 - DATE_LAYER_VERTICAL_OFFSET),
    DATE_LAYER_HORIZONTAL_OFFSET,
    60));
  
  date_layer_bottom = text_layer_create(GRect(
    0,
    (bounds.size.h - DATE_LAYER_BOTTOM_OFFSET),
    bounds.size.w,
    DATE_LAYER_BOTTOM_OFFSET));
  
  text_layer_set_background_color(date_layer_right, GColorClear);
  text_layer_set_text_color(date_layer_right, date_color);
  text_layer_set_text_alignment(date_layer_right, GTextAlignmentCenter);
  text_layer_set_font(date_layer_right, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  text_layer_set_background_color(date_layer_left, GColorClear);
  text_layer_set_text_color(date_layer_left, date_color);
  text_layer_set_text_alignment(date_layer_left, GTextAlignmentCenter);
  text_layer_set_font(date_layer_left, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  text_layer_set_background_color(date_layer_bottom, GColorClear);
  text_layer_set_text_color(date_layer_bottom, date_color);
  text_layer_set_text_alignment(date_layer_bottom, GTextAlignmentCenter);
  text_layer_set_font(date_layer_bottom, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  
  update_date();
  
  layer_add_child(window_layer, background_layer);
  layer_add_child(window_layer, text_layer_get_layer(date_layer_right));
  layer_add_child(window_layer, text_layer_get_layer(date_layer_left));
  layer_add_child(window_layer, text_layer_get_layer(date_layer_bottom));
  layer_add_child(window_layer, hands_layer);
  
  window_stack_push(watch_window, true);
}

static void deinit(void){
  tick_timer_service_unsubscribe();
  connection_service_unsubscribe();
  
  window_destroy(watch_window);
  text_layer_destroy(date_layer_right);
  text_layer_destroy(date_layer_left);
  text_layer_destroy(date_layer_bottom);
  layer_destroy(background_layer);
  layer_destroy(hands_layer);
}

int main(void){
  init();
  app_event_loop();
  deinit();
}