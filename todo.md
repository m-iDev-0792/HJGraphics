adjust up vector when current direction is close to (0,1,0)
gNormal RGB16F RGB
gAlbedo RGB RGB
glRoughnessMetallic RG RG
修改
GBuffer class ok
GBuffer write shader ok
GBuffer bind 
GBuffer uniform write 
PBRlighting shader ok
PBRIBL shader ok
normal reading ok
gAO binding point ok
# TODOs of HJGraphics
[x] 适配retina屏幕
- 窗口framebuffer尺寸变化需要通知renderer

### 工具

[x] 日志系统，设置颜色，是否保存日志文件

[x] cmake脚本，编译后自动拷贝dll库

[ ] 注释 文档系统

[ ] debug mesh

### 渲染
[ ] DrawBuffer class封装

[ ] renderable 反射系统 序列化 描述 update事件 

[ ] instance mesh

[ ] PBR模型加载, 模型转换

[ ] 窗口事件注册，lambda？

[ ] urgent! 移出GBuffer中的F0通道

### 场景管理

[ ] cull system, 相机剔除,阴影剔除，aabb

[ ] 排序系统, 对物体进行材质, 摄像机位置的排序

### 材质

[ ] 材质类型(opaque, transparent, forward, unlit ...)

### 优化
[ ] variable state(dirty or not)
[ ] copy on write, 例如mesh共用