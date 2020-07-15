| Render (platform agnostic)         | Render API (platform specific) |
| ---------------------------------- | ------------------------------ |
| 2D and 3D Render                   | Render Context                 |
| - Foward or deffered               | Swap Chain                     |
| Scene Graph                        | Framebuffer                    |
| Sorting                            | Vertex Buffer                  |
| Culling                            | Index Buffer                   |
| Materials                          | Texture                        |
| LOD (level of detail)              | Shader                         |
| Animation                          | States                         |
| Camera                             | Pipelines                      |
| VFX (eg. particle system)          | Render passes                  |
| PostFX (eg. color correction)      | Render Command Queue           |
| Other things                       |                                |
| - reflections or ambient occlusion |                                |

## Where do we start

OpenGL (since the simpliest and easiest)

1. build Render API
2. build Render
3. Now we have a base to render anything we want



## dll to static library

Since there are more complications in a .dll library we will convert our project to a static library



## 27 - Rendering Context

<a href="https://ibb.co/MPdx3nc"><img src="https://i.ibb.co/QY0319P/opengl1.png" alt="opengl1" border="0"></a>