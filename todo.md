# TODOs of HJGraphics

### refactor
- [ ] merge properties of some classes as XXXDescriptor

### 工具

- [x] 日志系统，设置颜色，是否保存日志文件
- [x] cmake脚本，编译后自动拷贝dll库
- [ ] 注释 文档系统
- [x] debug mesh
- [x] options for building HJGraphics as a static or dynamic lib

### 渲染
- [ ] DrawBuffer class封装
- [ ] renderable 反射系统 序列化 描述 update事件
- [ ] Material sort, mesh combine
- [ ] instance mesh
- [ ] PBR模型加载, 模型转换
- [ ] 窗口事件注册，lambda？
- [x] SSR
- [ ] Lens flare
- [x] Chromatic aberration
- [x] Blooming
- [x] 适配retina屏幕
- [ ] tile based rendering for non-shadow lights
- [ ] Replace IBL diffuse irradiance cube map with Spherical Harmonics

### 架构

- [ ] [scene graph](https://learnopengl.com/Guest-Articles/2021/Scene/Scene-Graph)
- [ ] culling system, [frustum culling](https://learnopengl.com/Guest-Articles/2021/Scene/Frustum-Culling)
- [ ] 排序系统, 对物体进行材质, 摄像机位置的排序
- [ ] ECS

### 材质

- [ ] 材质类型(opaque, transparent, forward, unlit ...)

### 优化
- [ ] dirty flag
- [ ] copy on write, 例如mesh 材质共用