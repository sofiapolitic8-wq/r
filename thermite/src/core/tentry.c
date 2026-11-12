#include "tentry.h"

int tentry(int argc, char** argv, size_t usr_size) {
  tenv env;

  env.config.argc = argc;
  env.config.argv = argv;
  env.config.vsync = true;
  env.config.running = true;
  env.config.fullscreen = false;
  env.config.resizable = true;
  env.config.aspect_ratio = 16 / 9.0f;
  env.config.fif = 2;
  env.config.fps_limit = 0;
  env.config.idle_fps = 0;
  env.config.title = "app";
  env.usr = malloc(usr_size);

  tlaunch(&env);

  env.wnd = twindow_create(&env, trender, tresize);
  env.kb = tkeyboard_create(env.wnd);
  env.ms = tmouse_create(env.wnd);
  if ((env.ctx = tcontext_create(env.wnd, env.config.vsync, env.config.fif)) == NULL) {
    printf("Error creating context.\n");
    exit(1);
  }
  tinit(&env);

  double next_frame = glfwGetTime();
  while (env.config.running) {
    if (env.ctx->swapchain_ok) {
      twindow_poll_input(env.wnd);
    } else {
      twindow_wait_input(env.wnd);
      continue;
    }

    tinput(&env);
    trender(&env);

    tkeyboard_update(env.kb);
    tmouse_update(env.ms);

    /* ---- frame limiter: sleep (cheap) then short spin (precise) ---- */
    int cap = env.config.fps_limit;
    if (env.config.idle_fps > 0 &&
        (glfwGetWindowAttrib(env.wnd->handle, GLFW_ICONIFIED) ||
         !glfwGetWindowAttrib(env.wnd->handle, GLFW_FOCUSED)))
      cap = env.config.idle_fps;
    if (cap > 0) {
      double step = 1.0 / cap;
      double now = glfwGetTime();
      next_frame += step;
      if (next_frame < now - step) next_frame = now; /* don't try to catch up */
      while ((now = glfwGetTime()) < next_frame) {
        double left = next_frame - now;
        if (left > 0.002) glfwWaitEventsTimeout(left - 0.001);
      }
    } else {
      next_frame = glfwGetTime();
    }
  }
  tcontext_wait_idle(env.ctx);

  tdestroy(&env);
  tcontext_destroy(env.ctx);
  tmouse_destroy(env.ms);
  tkeyboard_destroy(env.kb);
  twindow_destroy(env.wnd);
  free(env.usr);

  return 0;
}