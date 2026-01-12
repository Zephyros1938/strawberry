#version 460 core
out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoord;

uniform sampler2D texture1;

void main()
{
  // Hard-coded light direction (world space)
  vec3 lightDir = normalize(vec3(0.3, 1.0, 0.5));

  // Normalize interpolated normal
  vec3 N = normalize(Normal);

  // Lambertian diffuse term
  float diffuse = max(dot(N, lightDir), 0.0);

  // Hard-coded ambient term
  float ambient = 0.25;

  vec3 lighting = vec3(ambient + diffuse);

  vec4 albedo = texture(texture1, TexCoord);

  FragColor = vec4(albedo.rgb * lighting, albedo.a);
}
