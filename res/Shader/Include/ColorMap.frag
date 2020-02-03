uniform sampler2D u_ColorMapSampler;
#define COLOR_MAP_WIDTH 64
#define COLOR_MAP_HEIGHT 64

vec4 color_map_get(int x, int y)
{
	vec2 uv = vec2((x + 0.5) / COLOR_MAP_WIDTH, 1.0 - (y + 0.5) / COLOR_MAP_HEIGHT);
	return texture(u_ColorMapSampler, uv);
}
