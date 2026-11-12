#include "imgui_setup.h"

#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS

#include "cimgui/cimgui.h"
#include "cimgui/cimgui_impl.h"

#include "user.h"

void imgui_init(tenv* env) {
  tuser_data* usr = env->usr;

  igCreateContext(NULL);
  igImplVulkan_Init(&(ImGui_ImplVulkan_InitInfo){
      .ApiVersion = VK_API_VERSION_1_0,
      .Instance = env->ctx->instance,
      .PhysicalDevice = env->ctx->ph_device,
      .Device = env->ctx->device,
      .QueueFamily = env->ctx->queue_family,
      .Queue = env->ctx->queue,
      .DescriptorPool = env->ctx->descriptor_pool,
      .DescriptorPoolSize = 0,
      .MinImageCount = env->ctx->min_image_count,
      .ImageCount = env->ctx->fif,
      .PipelineCache = NULL,
      .PipelineInfoMain = {.RenderPass = env->ctx->renderpass,
                           .Subpass = 0,
                           .MSAASamples = VK_SAMPLE_COUNT_1_BIT},
      .UseDynamicRendering = false});
  igImplGlfw_InitForVulkan(env->wnd->handle, true);
  ImGuiIO* io = igGetIO_Nil();
  // io->MouseDrawCursor = true;

  for (int i = 0; i < NUM_FONT_SIZES; i++) {
    ImFontConfig icons_config = {.FontDataOwnedByAtlas = true,
                                 .OversampleH = 0,
                                 .OversampleV = 0,
                                 .GlyphMaxAdvanceX = FLT_MAX,
                                 .RasterizerDensity = 1,
                                 .RasterizerMultiply = 1,
                                 .EllipsisChar = 0,
                                 .MergeMode = true,
                                 .GlyphOffset = (ImVec2){0, 2 + i},
                                 .GlyphMinAdvanceX = 26.0f + i * 6};

    usr->imgui_data.mono_font[i] = ImFontAtlas_AddFontFromFileTTF(
        io->Fonts, "app/res/fonts/mono_regular.ttf", 20 + i * 4, NULL, NULL);

    ImFontAtlas_AddFontFromFileTTF(
        io->Fonts, "app/res/fonts/iconfont.ttf", 20 + i * 4, &icons_config,
        (const ImWchar[]){0xe900, 0xeaea, 0});

    usr->imgui_data.regular_font[i] = ImFontAtlas_AddFontFromFileTTF(
        io->Fonts, "app/res/fonts/regular_regular.ttf", 20 + i * 4, NULL, NULL);

    ImFontAtlas_AddFontFromFileTTF(
        io->Fonts, "app/res/fonts/iconfont.ttf", 20 + i * 4, &icons_config,
        (const ImWchar[]){0xe900, 0xeaea, 0});

    usr->imgui_data.mono_font_bold[i] = ImFontAtlas_AddFontFromFileTTF(
        io->Fonts, "app/res/fonts/mono_bold.ttf", 20 + i * 4, NULL, NULL);

    ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/iconfont.ttf",
                                   20 + i * 4, &icons_config,
                                   (const ImWchar[]){0xe900, 0xeaea, 0});

    usr->imgui_data.regular_font_bold[i] = ImFontAtlas_AddFontFromFileTTF(
        io->Fonts, "app/res/fonts/regular_bold.ttf", 20 + i * 4, NULL, NULL);

    ImFontAtlas_AddFontFromFileTTF(io->Fonts, "app/res/fonts/iconfont.ttf",
                                   20 + i * 4, &icons_config,
                                   (const ImWchar[]){0xe900, 0xeaea, 0});
  }
  
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io->IniFilename = NULL;

  ImGuiStyle* style = igGetStyle();
  style->DockingNodeHasCloseButton = false;
  style->WindowMenuButtonPosition = ImGuiDir_None;
  style->TabCloseButtonMinWidthUnselected = -1;
  style->WindowBorderSize = style->FrameBorderSize = style->ChildBorderSize =
      style->PopupBorderSize = style->TabBorderSize = 1;
  style->FramePadding = (ImVec2){8, 8};
  style->ItemSpacing = (ImVec2){4, 4};
  style->ItemInnerSpacing = (ImVec2){4, 4};
  style->WindowPadding = (ImVec2){4, 4};
  style->GrabMinSize = 18;
  style->FrameRounding = style->TabRounding = style->ChildRounding =
      style->GrabRounding = style->PopupRounding = style->ScrollbarRounding =
          style->WindowRounding = style->TreeLinesRounding = 3;
  style->ScrollbarSize = 10;
  style->DockingSeparatorSize = 1;
  style->ScrollbarPadding = 1;
  style->CellPadding.x = 2;

  /* Roshan.io theme: deep navy surfaces, violet accent, cyan highlights */
  igStyleColorsDark(style);
  style->FrameRounding = style->ChildRounding = style->GrabRounding =
      style->PopupRounding = style->ScrollbarRounding = style->TabRounding = 8;
  style->WindowRounding = 10;
  style->FramePadding = (ImVec2){10, 9};
  style->ItemSpacing = (ImVec2){6, 6};
  style->WindowBorderSize = style->ChildBorderSize = style->PopupBorderSize = 0;
  style->FrameBorderSize = 1;
  const ImVec4 bg      = {0.059f, 0.071f, 0.110f, 1.00f};
  const ImVec4 panel   = {0.086f, 0.102f, 0.157f, 1.00f};
  const ImVec4 field   = {0.114f, 0.133f, 0.200f, 1.00f};
  const ImVec4 hover   = {0.169f, 0.192f, 0.290f, 1.00f};
  const ImVec4 accent  = {0.486f, 0.361f, 0.973f, 1.00f};
  const ImVec4 accent2 = {0.360f, 0.255f, 0.820f, 1.00f};
  const ImVec4 accentD = {0.275f, 0.200f, 0.620f, 1.00f};
  ImVec4* c = style->Colors;
  c[ImGuiCol_Text] = (ImVec4){0.91f, 0.93f, 0.98f, 1.00f};
  c[ImGuiCol_TextDisabled] = (ImVec4){0.46f, 0.50f, 0.62f, 1.00f};
  c[ImGuiCol_WindowBg] = c[ImGuiCol_PopupBg] = panel;
  c[ImGuiCol_ChildBg] = (ImVec4){0, 0, 0, 0};
  c[ImGuiCol_Border] = (ImVec4){0.22f, 0.25f, 0.38f, 0.60f};
  c[ImGuiCol_BorderShadow] = (ImVec4){0, 0, 0, 0};
  c[ImGuiCol_FrameBg] = field;
  c[ImGuiCol_FrameBgHovered] = hover;
  c[ImGuiCol_FrameBgActive] = accentD;
  c[ImGuiCol_TitleBg] = c[ImGuiCol_TitleBgActive] = bg;
  c[ImGuiCol_CheckMark] = (ImVec4){0.36f, 0.88f, 0.96f, 1.00f};
  c[ImGuiCol_SliderGrab] = accent;
  c[ImGuiCol_SliderGrabActive] = (ImVec4){0.36f, 0.88f, 0.96f, 1.00f};
  c[ImGuiCol_Button] = accent2;
  c[ImGuiCol_ButtonHovered] = accent;
  c[ImGuiCol_ButtonActive] = accentD;
  c[ImGuiCol_Header] = field;
  c[ImGuiCol_HeaderHovered] = hover;
  c[ImGuiCol_HeaderActive] = accentD;
  c[ImGuiCol_Separator] = (ImVec4){0.22f, 0.25f, 0.38f, 0.70f};
  c[ImGuiCol_Tab] = c[ImGuiCol_TabDimmed] = field;
  c[ImGuiCol_TabHovered] = accent;
  c[ImGuiCol_TabSelected] = c[ImGuiCol_TabDimmedSelected] = accent2;
  c[ImGuiCol_TabSelectedOverline] = (ImVec4){0.36f, 0.88f, 0.96f, 1.00f};
  c[ImGuiCol_ScrollbarGrab] = hover;
  c[ImGuiCol_ScrollbarGrabHovered] = accent2;
  c[ImGuiCol_ScrollbarGrabActive] = accent;
  c[ImGuiCol_PlotHistogram] = (ImVec4){0.36f, 0.88f, 0.96f, 1.00f};
  c[ImGuiCol_ModalWindowDimBg] = (ImVec4){0, 0, 0, 0.7f};
}

void imgui_prerender() {
  igImplVulkan_NewFrame();
  igImplGlfw_NewFrame();
  igNewFrame();
}

void imgui_render(VkCommandBuffer cmd) {
  igImplVulkan_RenderDrawData(igGetDrawData(), cmd, NULL);
}

void imgui_destroy() {
  igImplGlfw_Shutdown();
  igImplVulkan_Shutdown();
  igDestroyContext(NULL);
}