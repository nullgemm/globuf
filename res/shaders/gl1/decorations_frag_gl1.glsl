#version 130
precision mediump float;

uniform float title_size;
uniform float button_size;
uniform float button_icon_size;
uniform float border_size;
uniform float width;
uniform float height;
uniform float hover;

void main()
{
	vec2 pos = gl_FragCoord.xy;
	float button_ya = 1.0 - step(height - (title_size - button_icon_size) / 2.0, pos.y);
	float button_yb = step(height - (title_size + button_icon_size) / 2.0, pos.y);
	float button_close_xa = step(width - border_size - (button_size + button_icon_size) / 2.0, pos.x);
	float button_close_xb = 1.0 - step(width - border_size - (button_size - button_icon_size) / 2.0, pos.x);
	float button_maximize_xa = step(width - border_size - (3.0 * button_size + button_icon_size) / 2.0, pos.x);
	float button_maximize_xb = 1.0 - step(width - border_size - (3.0 * button_size - button_icon_size) / 2.0, pos.x);
	float button_minimize_xa = step(width - border_size - (5.0 * button_size + button_icon_size) / 2.0, pos.x);
	float button_minimize_xb = 1.0 - step(width - border_size - (5.0 * button_size - button_icon_size) / 2.0, pos.x);
	float button_hover_ya = 1.0 - step(height - border_size, pos.y);
	float button_hover_yb = step(height - title_size + border_size, pos.y);
	float button_hover_xa = step(width - border_size - (hover * button_size), pos.x);
	float button_hover_xb = 1.0 - step(width - border_size - ((hover - 1.0) * button_size), pos.x);
	float close = button_ya * button_yb * button_close_xa * button_close_xb;
	float maximize = button_ya * button_yb * button_maximize_xa * button_maximize_xb;
	float minimize = button_ya * button_yb * button_minimize_xa * button_minimize_xb;
	float hover_state = button_hover_ya * button_hover_yb * button_hover_xa * button_hover_xb;
	gl_FragColor = (1.0 - close) * (1.0 - maximize) * (1.0 - minimize) * (1.0 - hover_state) * vec4(0.117, 0.117, 0.117, 1.0);
	gl_FragColor += (1.0 - close) * (1.0 - maximize) * (1.0 - minimize) * hover_state * vec4(0.164, 0.164, 0.164, 1.0);
	gl_FragColor += close * vec4(0.478, 0.125, 0.125, 1.0);
	gl_FragColor += maximize * vec4(0.188, 0.482, 0.184, 1.0);
	gl_FragColor += minimize * vec4(0.498, 0.470, 0.149, 1.0);
}
