# SIM-Projet
## Fonctions utiles
    - Max/Min
    - Floor/Ceil/Round
    - Mix(v, u, a) -> (1-a)*v + a*u
    - Smoothstep(e1, e2, x)
    - Step(e1, x)
    - Tan/Cos/Sin
    - Atan/Acos/Asin
    - smoothstep -> pour avoir 2 terrain différents d = length(p) smoothstep(0.5, 0.1, 0.5+0.1, d)
## camera
    - glm::lookAt(pos, centre, up=(0,1,0))
    - glm::perspective(fw=45,mind, maxd, aspect ration)