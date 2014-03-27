#ifndef MLDATALOG_H
#define MLDATALOG_H

#ifdef __cplusplus
extern "C"
{
#endif

enum DATALOG_CONTROL
{
    DATA_LOG_INIT = 0,
    DATA_LOG_ENABLE,
    DATA_LOG_PROCESS,
    DATA_LOG_DISABLE
};

void mlDataLogInit(void);
int mlDataLogOpen(void);
void mlDataLogClose(void);

void mlDataLogEnable(void);
void mlDataLogDisable(void);
void mlDataLogProcess(void);

#ifdef __cplusplus
}
#endif

#endif /* MLDATALOG_H */
