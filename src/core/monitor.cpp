#include "__monitor.hpp"

static void *monitorProgress(void *m);

af_Monitor *makeMonitor(af_Environment *env) {
    auto monitor = calloc(1, af_Monitor);
    monitor->env = env;
    pthread_mutex_init(&monitor->lock, nullptr);
    pthread_cond_init(&monitor->cond, nullptr);

    env->monitor = monitor;
    pthread_create(&monitor->thread, nullptr, monitorProgress, monitor);
    return monitor;
}

void freeMonitor(af_Environment *env) {
    af_Monitor *monitor = env->monitor;

    pthread_mutex_lock(&monitor->lock);
    monitor->exit = true;
    pthread_mutex_unlock(&monitor->lock);
    pthread_cond_signal(&monitor->cond);

    pthread_join(monitor->thread, nullptr);

    pthread_mutex_lock(&env->thread_lock);
    pthread_cond_destroy(&monitor->cond);
    pthread_mutex_destroy(&monitor->lock);
    free(monitor);
    env->monitor = nullptr;
    pthread_mutex_unlock(&env->thread_lock);
}

static void *monitorProgress(void *m) {
    af_Monitor *monitor = (af_Monitor *)m;
    af_Environment *env = monitor->env;
    pthread_mutex_lock(&monitor->lock);
    while (true) {
        if (isEnviromentExit(env)) {
            pthread_mutex_lock(&monitor->env->status_lock);
            monitor->env->status = core_exit;
            pthread_mutex_unlock(&monitor->env->status_lock);
        }

        pthread_cond_wait(&monitor->cond, &monitor->lock);
        if (monitor->exit)
            break;
    }
    pthread_mutex_unlock(&monitor->lock);
    return nullptr;
}
