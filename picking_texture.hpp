#ifndef PICKING_TEXTURE_HPP
#define PICKING_TEXTURE_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

class PickingTexture {
  public:
    struct PixelInfo {
        int object_id = 0;
        // i have to leave these here for now because I'm using rbg, later on switch to monotone and remove theses
        int draw_id = 0;
        int primitive_id = 0;
    };

    void initialize(unsigned int window_width_px, unsigned int window_height_px) {

        // Create the FBO
        glGenFramebuffers(1, &frame_buffer_gl_handle);
        glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_gl_handle);

        // Create the texture object for the primitive information buffer
        glGenTextures(1, &picking_texture_gl_handle);
        glBindTexture(GL_TEXTURE_2D, picking_texture_gl_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32UI, window_width_px, window_height_px, 0, GL_RGB_INTEGER,
                     GL_UNSIGNED_INT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, picking_texture_gl_handle, 0);

        // Create the texture object for the depth buffer
        glGenTextures(1, &depth_texture_gl_handle);
        glBindTexture(GL_TEXTURE_2D, depth_texture_gl_handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, window_width_px, window_height_px, 0, GL_DEPTH_COMPONENT,
                     GL_FLOAT, NULL);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture_gl_handle, 0);

        // Verify that the FBO is correct
        GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

        if (Status != GL_FRAMEBUFFER_COMPLETE) {
            printf("FB error, status: 0x%x\n", Status);
            exit(1);
        }

        // Restore the default framebuffer
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void enable_writing() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer_gl_handle); }

    void disable_writing() {
        // Bind back the default framebuffer
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    }

    PixelInfo read_pixel(unsigned int x, unsigned int y) {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, frame_buffer_gl_handle);

        glReadBuffer(GL_COLOR_ATTACHMENT0);

        PixelInfo pixel;
        glReadPixels(x, y, 1, 1, GL_RGB_INTEGER, GL_UNSIGNED_INT, &pixel);

        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        return pixel;
    }

  private:
    GLuint frame_buffer_gl_handle = 0;
    GLuint picking_texture_gl_handle = 0;
    GLuint depth_texture_gl_handle = 0;
};

#endif // PICKING_TEXTURE_HPP
