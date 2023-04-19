#pragma once

//  F = ma == a = F/m
// dv/dt = a = F/m == dx/dt = v
// acceleration = force / mass
// change in position = velocity * dt
// change in velocity = acceleration * dt

//오일러 적분
/*
void ExplicitEuler()
{
    double t = 0.0;
    float dt = 1.0f;

    float velocity = 0.0f;
    float position = 0.0f;
    float force = 10.0f;
    float mass = 1.0f;

    while (t <= 10.0) // 10.0kg
    {
        position = position + velocity * dt;
        velocity = velocity + (force / mass) * dt;
        ->반 오일러 적분
        velocity += acceleration * dt;
        position += velocity * dt;

        t += dt;
    }
}

    t=0:    position = 0      velocity = 0
    t=1:    position = 0      velocity = 10
    t=2:    position = 10     velocity = 20
    t=3:    position = 30     velocity = 30
    t=4:    position = 60     velocity = 40
    t=5:    position = 100    velocity = 50
    t=6:    position = 150    velocity = 60
    t=7:    position = 210    velocity = 70
    t=8:    position = 280    velocity = 80
    t=9:    position = 360    velocity = 90
    t=10:   position = 450    velocity = 100

    오일러 적분은 변화량이 시간에 걸쳐 일정할때만 100% 정확합니다.
    가속도가 일정하면 속도의 적분은 오류가 발생하지 않지만
    시간값으로 인해 오류가 발생합니다.

    s = ut + 0.5at^2
    s = 0.0*t + 0.5at^2
    s = 0.5(10)(10^2)
    s = 0.5(10)(100)
    s = 500 meters 
*/

/*
//반대로 RK4는 더 정화하지만 에너지 보존량이 점점 감소하게 된다
struct State
{
    float x;      // position
    float v;      // velocity
};

struct Derivative
{
    float dx;      // dx/dt = velocity
    float dv;      // dv/dt = acceleration
};

Derivative evaluate(const State& initial,
    double t,
    float dt,
    const Derivative& d)
{
    State state;
    state.x = initial.x + d.dx * dt;
    state.v = initial.v + d.dv * dt;

    Derivative output;
    output.dx = state.v;
    output.dv = acceleration(state, t + dt);
    return output;
}
float acceleration(const State& state, double t)
{
    const float k = 15.0f;
    const float b = 0.1f;
    return -k * state.x - b * state.v;
}
void integrate(State& state,
    double t,
    float dt)
{
    Derivative a, b, c, d;

    a = evaluate(state, t, 0.0f, Derivative());
    b = evaluate(state, t, dt * 0.5f, a);
    c = evaluate(state, t, dt * 0.5f, b);
    d = evaluate(state, t, dt, c);

    float dxdt = 1.0f / 6.0f *
        (a.dx + 2.0f * (b.dx + c.dx) + d.dx);

    float dvdt = 1.0f / 6.0f *
        (a.dv + 2.0f * (b.dv + c.dv) + d.dv);

    state.x = state.x + dxdt * dt;
    state.v = state.v + dvdt * dt;
}
*/