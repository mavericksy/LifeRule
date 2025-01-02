#include <SDL2/SDL.h>
#include <stdio.h>
#include <time.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600
#define COLOUR_LIGHT 0xC7F0D8
#define COLOUR_DARK1 0x43523D
#define COLOUR_DARK2 0xf15131
#define COLOUR_DARK3 0x41f131

#define DELTA_T 0.1;

#define ATOMS 1000
#define ATOM_RADIUS 2.3

typedef struct Atom {
    double x, y, vx, vy, radius;
    uint32_t colour;
} Atom;

struct Atom atoms1[ATOMS];
struct Atom atoms2[ATOMS];
struct Atom atoms3[ATOMS];

double random_double(double l_bnd, double h_bnd, long max_rand) {
    return l_bnd + (h_bnd - l_bnd) * (rand() % max_rand) / max_rand;
}

void FillAtom(SDL_Surface *surface, struct Atom atom) {
    double low_x = atom.x - atom.radius;
    double low_y = atom.y - atom.radius;
    double high_x = atom.x + atom.radius;
    double high_y = atom.y + atom.radius;
    double radsq = atom.radius * atom.radius;
    //
    for (double x = low_x; x < high_x; x++) {
      for (double y = low_y; y < high_y; y++) {
        double x_off = (x - atom.x);
        double y_off = (y - atom.y);
        double center_dist_sq = x_off * x_off + y_off * y_off;
        if (center_dist_sq < radsq) {
          SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
          SDL_FillRect(surface, &pixel, atom.colour);
        }
      }
    }
}

void checkEdges(struct Atom *body) {
   if (body->x + body->radius > SCREEN_WIDTH) {
    body->x = SCREEN_WIDTH - body->radius;
    body->vx = -body->vx;
  }
  //
  if (body->y + body->radius > SCREEN_HEIGHT) {
    body->y = SCREEN_HEIGHT - body->radius;
    body->vy = -body->vy;
  }
  //
  if (body->y - body->radius < 0) {
    body->y = body->radius;
    body->vy = -body->vy;
  }
  //
  if (body->x - body->radius < 0) {
    body->x = body->radius;
    body->vx = -body->vx;
  }
}


void rule(struct Atom atoms1[ATOMS], struct Atom atoms2[ATOMS], double g) {
    //
    for (int i = 0; i < ATOMS; i++) {
      double fx = 0;
      double fy = 0;
      Atom* a = &atoms1[i];
      for (int j = 0; j < ATOMS; j++) {
        Atom* b = &atoms2[j];
        double dx = +(a->x - b->x);
        double dy = +(a->y - b->y);
        double dist = dx * dx + dy * dy;
        if (dist > 0 && dist < 500) {
          double f = g * 1 / sqrt(dist);
          fx += (f * dx);
          fy += (f * dy);
        }
      }
      //
      a->vx = (a->vx + fx) * 0.2;
      a->vy = (a->vy + fy) * 0.2;
      a->x += a->vx;
      a->y += a->vy;
      checkEdges(a);
    }
}

int check = 600;

double random_force(int mod, int r, int c, double cur[3][3]) {
    cur[r][c] = mod < check ? cur[r][c] : random_double(-3, 3, 1000000l);
    return cur[r][c];
}

int main(void) {
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_INIT ERROR: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window* window =
        SDL_CreateWindow("Life Rule",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED,
                         SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if(!window) {
        printf("ERROR: %s", SDL_GetError());
        SDL_Quit();
        exit(1);
    }
    //
    srand(time(NULL));
    //
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    //
    SDL_Rect back = (SDL_Rect){0,0,SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Event event;
    //
    for(int i = 0;i<ATOMS;i++) {
        double x = random_double(0, SCREEN_WIDTH, 1000000l);
        double y = random_double(0, SCREEN_HEIGHT, 1000000l);
        atoms1[i] = (Atom){x,y,0,0,ATOM_RADIUS,COLOUR_DARK1};
        FillAtom(surface, atoms1[i]);
    }
    for(int i = 0;i<ATOMS;i++) {
        double x = random_double(0, SCREEN_WIDTH, 1000000l);
        double y = random_double(0, SCREEN_HEIGHT, 1000000l);
        atoms2[i] = (Atom){x,y,0,0,ATOM_RADIUS,COLOUR_DARK2};
        FillAtom(surface, atoms2[i]);
    }
   for(int i = 0;i<ATOMS;i++) {
        double x = random_double(0, SCREEN_WIDTH, 1000000l);
        double y = random_double(0, SCREEN_HEIGHT, 1000000l);
        atoms3[i] = (Atom){x,y,0,0,ATOM_RADIUS,COLOUR_DARK3};
        FillAtom(surface, atoms3[i]);
    }

   double current_force[3][3] = {
   {random_double(-2, 2, 1000000l),-1,-1},
   {-1,random_double(-2, 2, 1000000l),-1},
   {-1,-1,random_double(-2, 2, 1000000l)}};

   int tick_count = 0;
    //
    int32_t run = 1;
    while (run) {
        //
        while (SDL_PollEvent(&event)) {
          if (event.type == SDL_QUIT) {
            run = 0;
          }
        }
        //
        SDL_FillRect(surface, &back, COLOUR_LIGHT);
        //
        for(int i = 0;i<ATOMS;i++) {
            FillAtom(surface, atoms1[i]);
            FillAtom(surface, atoms2[i]);
            FillAtom(surface, atoms3[i]);
        }
        //
        tick_count++;
        //
        rule(atoms1, atoms1, random_force(tick_count, 0, 0, current_force));
        rule(atoms1, atoms2, random_force(tick_count, 0, 1, current_force));
        rule(atoms1, atoms3, random_force(tick_count, 0, 2, current_force));
        //
        rule(atoms2, atoms1, random_force(tick_count, 1, 0, current_force));
        rule(atoms2, atoms2, random_force(tick_count, 1, 1, current_force));
        rule(atoms2, atoms3, random_force(tick_count, 1, 2, current_force));
        //
        rule(atoms3, atoms1, random_force(tick_count, 2, 0, current_force));
        rule(atoms3, atoms2, random_force(tick_count, 2, 1, current_force));
        rule(atoms3, atoms3, random_force(tick_count, 2, 2, current_force));
        //
        if(tick_count >= check) {
            tick_count = 0;
        }
        //
        SDL_UpdateWindowSurface(window);
        //SDL_Delay(10);
    }
    //
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
