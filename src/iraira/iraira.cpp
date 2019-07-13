#include <cstdio>
#include <cmath>
#include <string>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <chrono>

#define GLFW_INCLUDE_GLU  // GLUライブラリを使用するのに必要
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "common.h"

static int WIN_WIDTH   = 640;                 // ウィンドウの幅
static int WIN_HEIGHT  = 480;                 // ウィンドウの高さ
static const char *WIN_TITLE = "イライラ棒";     // ウィンドウのタイトル

static const std::string iraira_way = std::string(DATA_DIRECTORY) + "iraira_way.png";
static const std::string iraira_clear = std::string(DATA_DIRECTORY) + "iraira_clear.png";
static const std::string iraira_crash = std::string(DATA_DIRECTORY) + "iraira_crash.png";
static const std::string iraira_way_mask = std::string(DATA_DIRECTORY) + "iraira_way_mask.png";
static const std::string iraira_start_mask = std::string(DATA_DIRECTORY) + "iraira_start_mask.png";
static const std::string iraira_end_mask = std::string(DATA_DIRECTORY) + "iraira_end_mask.png";
static const std::string iraira_retry_mask = std::string(DATA_DIRECTORY) + "iraira_retry_mask.png";
static const std::string iraira_title = std::string(DATA_DIRECTORY) + "iraira_title.png";

static GLuint textureId = 0u;

bool IS_MOUSE_PRESSED = false;
int g_px, g_py;
bool started = false;
bool retry = false;

unsigned char* load_mask(std::string mask)
{
    unsigned char* data;
    int x, y, comps;
    data = stbi_load(mask.c_str(), &x, &y, &comps, 1);
    return data;
}

unsigned char* way_data = load_mask(iraira_way_mask);
unsigned char* start_data = load_mask(iraira_start_mask);
unsigned char* end_data = load_mask(iraira_end_mask);
unsigned char* retry_data = load_mask(iraira_retry_mask);

void initializeGL(std::string tex_file) {
    // 背景色の設定 (黒)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // 深度テストの有効化
    glEnable(GL_DEPTH_TEST);

    // テクスチャの有効化
    glEnable(GL_TEXTURE_2D);

    // テクスチャの設定
    int texWidth, texHeight, channels;
    unsigned char *bytes = stbi_load(tex_file.c_str(), &texWidth, &texHeight, &channels, STBI_rgb_alpha);
    if (!bytes) {
        fprintf(stderr, "Failed to load image file: %s\n", tex_file.c_str());
        exit(1);
    }

    // テクスチャの生成と有効化
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    // 単純なテクスチャの転送
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, texWidth, texHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);

    // テクスチャの画素値参照方法の設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // テクスチャ境界の折り返し設定
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // テクスチャの無効化
    glBindTexture(GL_TEXTURE_2D, 0);

    // ロードした画素情報の破棄
    stbi_image_free(bytes);
}

void paintGL(){
    // 背景色と深度値のクリア
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glOrtho(0.0, WIN_WIDTH, WIN_HEIGHT, 0.0, -1.0, 1.0);

    glBindTexture(GL_TEXTURE_2D, textureId);
    glBegin(GL_POLYGON);

    glTexCoord2f(0.0f, 1.0f); glVertex2d(0, WIN_HEIGHT);
    glTexCoord2f(0.0f, 0.0f); glVertex2d(0, 0);
    glTexCoord2f(1.0f, 0.0f); glVertex2d(WIN_WIDTH, 0);
    glTexCoord2f(1.0f, 1.0f); glVertex2d(WIN_WIDTH, WIN_HEIGHT);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void paint_sp(GLFWwindow *window){
    paintGL();
    glfwSwapBuffers(window);
    glfwPollEvents();
}

bool is_white(unsigned char* data, int i, int j){
  if((i < WIN_HEIGHT && i >= 0) && (j < WIN_WIDTH && j >= 0)){ 
  int x = WIN_WIDTH;
  unsigned bytePerPixel = 1;
  unsigned char* pixelOffset = data + (x * i + j) * bytePerPixel;
  unsigned char v = pixelOffset[0];
  if (v == 0xFF){
      return true;
  }
  }
  return false;
}

void resizeGL(GLFWwindow *window, int width, int height) {
    // ユーザ管理のウィンドウサイズを変更
    WIN_WIDTH = width;
    WIN_HEIGHT = height;
    
    // GLFW管理のウィンドウサイズを変更
    glfwSetWindowSize(window, WIN_WIDTH, WIN_HEIGHT);

    // 実際のウィンドウサイズ (ピクセル数) を取得
    int renderBufferWidth, renderBufferHeight;
    glfwGetFramebufferSize(window, &renderBufferWidth, &renderBufferHeight);

    // ビューポート変換の更新
    glViewport(0, 0, renderBufferWidth, renderBufferHeight);
}

void mouseEvent(GLFWwindow *window, int button, int action, int mods) {
    // マウスが押されたかどうかの判定
    // printf("Mouse: %s\n", action == GLFW_PRESS ? "Press" : "Release");
    IS_MOUSE_PRESSED = (action == GLFW_PRESS);

    if(IS_MOUSE_PRESSED){
    // クリックされた位置を取得
    double px, py;
    glfwGetCursorPos(window, &px, &py);
    // printf("Mouse at: (%d, %d)\n", (int)px, (int)py);

    started = is_white(start_data, (int)py, (int)px);
    retry = is_white(retry_data, (int)py, (int)px);
    }
}

void motionEvent(GLFWwindow *window, double px, double py) {
    g_px = int(px);
    g_py = int(py);
}

void process(GLFWwindow *window){
        // OpenGLを初期化
    initializeGL(iraira_title);
    started = false;
    while (glfwWindowShouldClose(window) == GL_FALSE) {
        // 描画
        paint_sp(window);
        if(started)
        {
        auto chrono_start = std::chrono::system_clock::now();
        initializeGL(iraira_way);
        while (glfwWindowShouldClose(window) == GL_FALSE) {
            paint_sp(window);

            if(!is_white(way_data, g_py, g_px))
            {
                initializeGL(iraira_crash);
                retry = false;
                while (glfwWindowShouldClose(window) == GL_FALSE) {
                paint_sp(window);
                if(retry)
                {
                    process(window);
                }
                }
            }
            else if(is_white(way_data, g_py, g_px) && is_white(end_data, g_py, g_px))
            {
                auto chrono_end = std::chrono::system_clock::now();
                std::cout << "Elapsed time(C++):" << std::chrono::duration_cast<std::chrono::milliseconds>(chrono_end - chrono_start).count() << "[ms]" << std::endl;
                initializeGL(iraira_clear);
                retry = false;
                while (glfwWindowShouldClose(window) == GL_FALSE) {
                paint_sp(window);
                if(retry)
                {
                    process(window);
                }
                }
            }
        }
        }

    }

}

int main(int argc, char **argv) {
    // OpenGLを初期化する
    if (glfwInit() == GL_FALSE) {
        fprintf(stderr, "Initialization failed!\n");
        return 1;
    }

    // Windowの作成
    GLFWwindow *window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, WIN_TITLE,
                                          NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Window creation failed!");
        glfwTerminate();
        return 1;
    }

    // OpenGLの描画対象にWindowを追加
    glfwMakeContextCurrent(window);

    // ウィンドウのリサイズを扱う関数の登録
    glfwSetWindowSizeCallback(window, resizeGL);

    glfwSetMouseButtonCallback(window, mouseEvent);

    glfwSetCursorPosCallback(window, motionEvent);

    process(window);
}