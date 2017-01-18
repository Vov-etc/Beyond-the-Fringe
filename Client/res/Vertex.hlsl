attribute vec2 v_coord;
//attribute vec4 v_col;
uniform vec2 move;
varying vec4 col;

void main(void) {
    col = vec4((vec2(1, 1) + v_coord) / 2, 0.1, 0.5);
    gl_Position = vec4(v_coord / 2 + move, 1, 1);
}
