#include "title_screen.h"
#include <math.h>

#include "../network/server.h"
#include "../user.h"

/* ---------- Roshan.io animated 3D logo ----------
 * Each letter is drawn as a stack of offset copies (extrusion) that leans
 * toward the mouse and sways on its own, with a travelling wave, colour
 * shimmer and a soft glow. Pure ImGui draw-list: no extra textures/RAM. */
static ImU32 col32(float r, float g, float b, float a) {
  return ((ImU32)(a * 255) << 24) | ((ImU32)(b * 255) << 16) |
         ((ImU32)(g * 255) << 8) | (ImU32)(r * 255);
}

static void draw_logo_3d(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  ImGuiIO* io = igGetIO_Nil();
  ImDrawList* dl = igGetWindowDrawList();
  ImFont* font = usr->imgui_data.regular_font_bold[FONT_SIZE_LARGE];
  const char* text = "Roshan.io";
  const int n = 9;

  float t = (float)glfwGetTime();
  float size = ctx->size[1] * 0.14f;
  if (size > 110) size = 110;
  if (size < 48) size = 48;
  float scale = size / font->LegacySize;

  /* measure letters */
  float w[9], total = 0;
  for (int i = 0; i < n; i++) {
    char ch[2] = {text[i], 0};
    ImVec2 sz;
    igPushFont(font, font->LegacySize);
    igCalcTextSize(&sz, ch, NULL, false, -1);
    igPopFont();
    w[i] = sz.x * scale;
    total += w[i];
  }

  /* tilt: slow auto-sway + lean toward mouse */
  float cx = ctx->size[0] * 0.5f, base_y = ctx->size[1] * 0.5f - size * 2.4f;
  float mx = (io->MousePos.x - cx) / (ctx->size[0] * 0.5f);
  float my = (io->MousePos.y - base_y) / (ctx->size[1] * 0.5f);
  if (mx < -1) mx = -1; if (mx > 1) mx = 1;
  if (my < -1) my = -1; if (my > 1) my = 1;
  float tilt_x = sinf(t * 0.9f) * 0.55f - mx * 0.9f;
  float tilt_y = 0.55f + cosf(t * 0.7f) * 0.15f - my * 0.3f;

  const int layers = 16;
  float depth = size * 0.045f;
  float x = cx - total * 0.5f;

  for (int i = 0; i < n; i++) {
    char ch[2] = {text[i], 0};
    float wave = sinf(t * 2.6f + i * 0.65f);
    float py = base_y + wave * size * 0.09f;
    float hue = 0.5f + 0.5f * sinf(t * 1.4f + i * 0.5f); /* 0..1 */
    bool io_part = i >= 6; /* ".io" */

    /* glow */
    for (int g = 3; g >= 1; g--) {
      float o = g * size * 0.035f;
      ImU32 gc = io_part ? col32(0.486f, 0.361f, 0.973f, 0.07f)
                         : col32(0.36f, 0.88f, 0.96f, 0.06f);
      for (int k = 0; k < 4; k++) {
        float ax = (k < 2 ? -o : o), ay = (k % 2 ? -o : o);
        ImDrawList_AddText_FontPtr(dl, font, size, (ImVec2){x + ax, py + ay},
                                   gc, ch, NULL, 0, NULL);
      }
    }

    /* extrusion (back to front) */
    for (int d = layers; d >= 1; d--) {
      float f = (float)d / layers;
      ImU32 c = io_part ? col32(0.20f - 0.10f * f, 0.12f - 0.07f * f, 0.55f - 0.30f * f, 1)
                        : col32(0.10f - 0.05f * f, 0.30f - 0.18f * f, 0.45f - 0.25f * f, 1);
      ImDrawList_AddText_FontPtr(
          dl, font, size,
          (ImVec2){x + tilt_x * depth * d, py + tilt_y * depth * d}, c, ch,
          NULL, 0, NULL);
    }

    /* front face */
    ImU32 front = io_part
        ? col32(0.55f + 0.25f * hue, 0.42f + 0.25f * hue, 1.0f, 1)
        : col32(0.80f + 0.20f * hue, 0.92f + 0.08f * hue, 1.0f, 1);
    ImDrawList_AddText_FontPtr(dl, font, size, (ImVec2){x, py}, front, ch,
                               NULL, 0, NULL);
    x += w[i];
  }
}

void ui_title_screen_init(tenv* env) {}

void ui_title_screen(tenv* env) {
  tuser_data* usr = env->usr;
  tcontext* ctx = env->ctx;
  user_settings* usrs = &usr->usrs;
  ImGuiStyle* style = igGetStyle();
  ImGuiIO* io = igGetIO_Nil();
  game_data* gdata = &usr->gdata;
  
  // version
  char version_str[16] = {0};
  sprintf(version_str, "v%s", APP_VERSION);
  ImVec2 vtxtsz; igCalcTextSize(&vtxtsz, version_str, NULL, false, -1);
  igSetCursorPosX(ctx->size[0] - vtxtsz.x - style->WindowPadding.x);
  igPushFont(usr->imgui_data.regular_font[FONT_SIZE_SMALL],
             usr->imgui_data.regular_font[FONT_SIZE_SMALL]->LegacySize);
  igTextColored((ImVec4){0.36f, 0.88f, 0.96f, 1}, version_str);
  igPopFont();

  draw_logo_3d(env);

  igPushFont(usr->imgui_data.regular_font[usrs->ui_font_size],
             usr->imgui_data.regular_font[usrs->ui_font_size]->LegacySize);

  usr->r->global.bg_opacity = 0;
  usr->r->global.bd_opacity = 0;
  usr->r->global.minimap_opacity = 0;

  float frame_height = igGetFrameHeight();

  float logo_size = 400;
  float logo_gap = 5;

  igPushFont(usr->imgui_data.mono_font[usrs->ui_font_size],
             usr->imgui_data.mono_font[usrs->ui_font_size]->LegacySize);
  igSetCursorPosX(ctx->size[0] / 2.0f - logo_size / 2);
  igSetCursorPosY(ctx->size[1] / 2.0f -
                  (igGetFrameHeight() - style->ItemSpacing.x) * 3);
  int tot_sec = (int)usrs->play_time;
  int hours = tot_sec / 3600;
  int minutes = (tot_sec % 3600) / 60;
  int seconds = tot_sec % 60;
  igTextColored((ImVec4){1, 1, 1, 0.5f}, "\ue99e");
  igSameLine(0, -1);
  igTextColored((ImVec4){1, 1, 1, 0.5f}, "%d", usrs->score);
  igSetCursorPosX(ctx->size[0] / 2.0f - logo_size / 2);
  igSetCursorPosY(ctx->size[1] / 2.0f -
                  (igGetFrameHeight() - style->ItemSpacing.x) * 2);
  igTextColored((ImVec4){1, 1, 1, 0.5f}, "\ueaeb");
  igSameLine(0, -1);
  igTextColored((ImVec4){1, 1, 1, 0.5f}, "%d", usrs->kills);
  igSetCursorPosX(ctx->size[0] / 2.0f - logo_size / 2);
  igSetCursorPosY(ctx->size[1] / 2.0f -
                  (igGetFrameHeight() - style->ItemSpacing.x));
  igTextColored((ImVec4){1, 1, 1, 0.6}, "\ue952");
  igSameLine(0, -1);
  igTextColored((ImVec4){1, 1, 1, 0.6}, "%02d:%02d:%02d", hours, minutes,
                seconds);
  igPopFont();

  igSetCursorPosX(ctx->size[0] / 2.0f - logo_size / 2);
  igSetCursorPosY(ctx->size[1] / 2.0f + style->ItemSpacing.y);
  igPushItemWidth(logo_size);
  igPushStyleColor_Vec4(ImGuiCol_FrameBg,
                        (ImVec4){0.114f, 0.133f, 0.200f, 1.0f});
  igInputTextWithHint("##nickname_input", "Nickname", usrs->nickname,
                      MAX_NICKNAME_LEN + 1, ImGuiInputTextFlags_None, NULL,
                      NULL);
  igSetCursorPosX(ctx->size[0] / 2.0f - logo_size / 2);
  igSetCursorPosY(ctx->size[1] / 2.0f + style->ItemSpacing.y * 2 +
                  frame_height);
  igInputTextWithHint("##ipv4_input", "IPv4:Port", usrs->ipv4, MAX_IPV4_LEN + 1,
                      ImGuiInputTextFlags_None, NULL, NULL);
  igPopStyleColor(1);
  igPopItemWidth();
  igSetCursorPosX(ctx->size[0] / 2.0f - logo_size / 2);
  igSetCursorPosY(ctx->size[1] / 2.0f + style->ItemSpacing.y * 3 +
                  frame_height * 2);

  if (igButton("\uea1c Play", (ImVec2){logo_size})) {
    usr->gdata.conn = CONNECTING;
    usr->gdata.curr_screen = PLAYING;
    glfwSetTime(0);
    server_connect(env);
  }
  igSetCursorPosX(ctx->size[0] / 2.0f - logo_size / 2);
  igSetCursorPosY(ctx->size[1] / 2.0f + style->ItemSpacing.y * 4 +
                  frame_height * 3);
  if (igButton("\ue90c Skin editor",
               (ImVec2){logo_size / 2 - style->ItemSpacing.x / 2}))
    usr->gdata.curr_screen = SKIN_EDITOR;
  igSameLine(0, -1);
  if (igButton("\ue991 Settings",
               (ImVec2){logo_size / 2 - style->ItemSpacing.x / 2})) {
    usr->gdata.curr_screen = SETTINGS;
  }
  igSetCursorPosX(ctx->size[0] / 2.0f - logo_size / 2);
  igSetCursorPosY(ctx->size[1] / 2.0f + style->ItemSpacing.y * 5 +
                  frame_height * 4);
  if (igButton("\ue9b6 Quit", (ImVec2){logo_size})) {
    env->config.running = false;
    save_user_settings(usrs);
  }

  igPopFont();

  /* ---- credit footer ---- */
  {
    igPushFont(usr->imgui_data.regular_font[FONT_SIZE_SMALL],
               usr->imgui_data.regular_font[FONT_SIZE_SMALL]->LegacySize);
    const char* l1 = APP_CREDIT;
    const char* l2 = APP_BASE_CREDIT;
    ImVec2 s1, s2;
    igCalcTextSize(&s1, l1, NULL, false, -1);
    igCalcTextSize(&s2, l2, NULL, false, -1);
    igSetCursorPosX(ctx->size[0] / 2.0f - s1.x / 2);
    igSetCursorPosY(ctx->size[1] - s1.y - s2.y - style->WindowPadding.y * 3);
    igTextColored((ImVec4){0.486f, 0.361f, 0.973f, 1}, "%s", l1);
    igSetCursorPosX(ctx->size[0] / 2.0f - s2.x / 2);
    igTextColored((ImVec4){0.46f, 0.50f, 0.62f, 1}, "%s", l2);
    igPopFont();
  }
}

void ui_title_screen_destroy(tenv* env) {}
