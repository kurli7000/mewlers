

Mesh *new_mesh_cube(Material *m, float xs, float ys, float zs);
Mesh *new_mesh_camera(Material *m);
Mesh *new_mesh_icosohedron(Material *m, float size);
Mesh *new_mesh_maasto(Material *m, Lfb *field, int polys, float kork, float taso0);
Mesh *new_mesh_hemisphere(Material *m, int us, int vs, float size);
Mesh *new_mesh_sphere(Material *m, int us, int vs, float size);
Mesh *new_mesh_xube(Material *m, float x0, float y0, float z0, float x1, float y1, float z1);
Mesh *new_mesh_zube(Material *m, float x0, float y0, float z0, float x1, float y1, float z1, int xg, int yg, int zg);
Mesh *mesh_levitaedget(Mesh *o, float k);
Mesh *new_mesh_matto(Mesh *o, Material *m, float xres, float yres);


