#pragma once

#include <directxmath.h>
#include <Core/Window/Window.h>

namespace Kraid
{
    
    using namespace DirectX;
    
#define W_KEY 0x57
#define A_KEY 0x41
#define S_KEY 0x53
#define D_KEY 0x44
#define CAM_MAX_PITCH 90.0f
    
    struct Camera
    {
        XMFLOAT3 position = {0,0,0};
        XMFLOAT3 view_direction = {0,0,1};
        XMFLOAT3 up_direction = {0,1,0};
        
        XMFLOAT3 right_direction = {0,0,0};
        
        const float keyboard_intensity = 0.01f;//TODO(Tiago): Might Need some tunning
        const float mouse_intensity = 0.05f;//TODO(Tiago): Might Need some tunning
        
        mutable bool has_changed = true;
        mutable XMMATRIX view_matrix;
        
        float pitch = 0; //NOTE(Tiago): x-axis rotation
        float yaw = 0;   //NOTE(Tiago): y-axis rotation
        
        bool is_mouse_button_pressed = false; //NOTE(Tiago): Used to controll if the mouse button is down
        bool is_first_capture = true; //NOTE(Tiago): Indicates weather this is the first time the mouse position is captured
        int previous_mouse_x;
        int previous_mouse_y;
        
        bool w_key_down = false;
        bool s_key_down = false;
        bool a_key_down = false;
        bool d_key_down = false;

    private:
        Window* window;

    public:
        Camera(Window& window, const XMFLOAT3& position = {0,0,1}, const XMFLOAT3& view_direction = {0,0,-1});
        XMMATRIX ViewMatrix() const;
        void Rotate();
        void Rotate(float pitch, float yaw);
        void DeltaRotate(float pitch_delta = 0, float yaw_delta = 0);
        void Move(float x_delta = 0, float y_delta =0, float z_delta = 0);
        void SetPosition(float x, float y, float z);
        void Forward(float intensity = 1.0f);
        void Backward(float intensity = 1.0f);
        void Left(float intensity = 1.0f);
        void Right(float intensity = 1.0f);
        
        private:
        void UpdateRightDirection();
        void HookControlls();
        void HookMouseControlls();
        void HookKeyboardControlls();
    };
    
}
