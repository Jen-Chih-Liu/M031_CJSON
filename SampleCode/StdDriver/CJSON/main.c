/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Show how to set GPIO pin mode and use pin data input/output control.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"
#include "malloc.h"
#include "cJSON.h"
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*----------------------------------------------------------------------*/
    /* Init I/O Multi-function                                              */
    /*----------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}


/*----------------------------------------------------------------------*/
/* Init UART0                                                           */
/*----------------------------------------------------------------------*/
void UART0_Init(void)
{
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}

int32_t main(void)
{
    cJSON_Hooks hooks;
    cJSON *cjson_test = NULL;
    cJSON *cjson_name = NULL;
    cJSON *cjson_age = NULL;
    cJSON *cjson_weight = NULL;
    cJSON *cjson_address = NULL;
    cJSON *cjson_address_country = NULL;
    cJSON *cjson_address_zipcode = NULL;
    cJSON *cjson_skill = NULL;
    cJSON *cjson_student = NULL;
    int    skill_array_size = 0, i = 0;
    cJSON *cjson_skill_item = NULL;

    char *str = NULL;
    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();


    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();
    my_mem_init(0); //initial memory
    printf("start mem usage:%d\n\r", my_mem_perused(0));
    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
    hooks.malloc_fn = mymalloc_fix0;
    hooks.free_fn = myfree_fix0;
    cJSON_InitHooks(&hooks);
    printf("check mem usage:%d\n\r", my_mem_perused(0));

    //crate cjson
    cjson_test = cJSON_CreateObject();


    cJSON_AddStringToObject(cjson_test, "name", "mculover666");


    cJSON_AddNumberToObject(cjson_test, "age", 22);


    cJSON_AddNumberToObject(cjson_test, "weight", 55.5);

    cjson_address = cJSON_CreateObject();
    cJSON_AddStringToObject(cjson_address, "country", "China");
    cJSON_AddNumberToObject(cjson_address, "zip-code", 111111);
    cJSON_AddItemToObject(cjson_test, "address", cjson_address);

    cjson_skill = cJSON_CreateArray();
    cJSON_AddItemToArray(cjson_skill, cJSON_CreateString("C"));
    cJSON_AddItemToArray(cjson_skill, cJSON_CreateString("Java"));
    cJSON_AddItemToArray(cjson_skill, cJSON_CreateString("Python"));
    cJSON_AddItemToObject(cjson_test, "skill", cjson_skill);


    cJSON_AddFalseToObject(cjson_test, "student");


    str = cJSON_Print(cjson_test);
    printf("json string:%s\n", str);
    printf("length:%d\n", strlen(str));

    cJSON_Delete(cjson_test);


    //cJSON_free(str);
    //printf("a:%d\n\r",my_mem_perused(0));
    printf("a:%d\n\r", my_mem_perused(0));

    //parse cjson
    cjson_test = cJSON_Parse(str);
    cJSON_free(str);
    printf("a:%d\n\r", my_mem_perused(0));

    if (cjson_test == NULL)
    {
        printf("parse fail.\n");
        return -1;
    }

    cjson_name = cJSON_GetObjectItem(cjson_test, "name");
    cjson_age = cJSON_GetObjectItem(cjson_test, "age");
    cjson_weight = cJSON_GetObjectItem(cjson_test, "weight");

    printf("name: %s\n", cjson_name->valuestring);
    printf("age:%d\n", cjson_age->valueint);
    printf("weight:%.1f\n", cjson_weight->valuedouble);


    cjson_address = cJSON_GetObjectItem(cjson_test, "address");
    cjson_address_country = cJSON_GetObjectItem(cjson_address, "country");
    cjson_address_zipcode = cJSON_GetObjectItem(cjson_address, "zip-code");
    printf("address-country:%s\naddress-zipcode:%d\n", cjson_address_country->valuestring, cjson_address_zipcode->valueint);


    cjson_skill = cJSON_GetObjectItem(cjson_test, "skill");
    skill_array_size = cJSON_GetArraySize(cjson_skill);
    printf("skill:[");

    for (i = 0; i < skill_array_size; i++)
    {
        cjson_skill_item = cJSON_GetArrayItem(cjson_skill, i);
        printf("%s,", cjson_skill_item->valuestring);
    }

    printf("\b]\n");


    cjson_student = cJSON_GetObjectItem(cjson_test, "student");

    if (cjson_student->valueint == 0)
    {
        printf("student: false\n");
    }
    else
    {
        printf("student:error\n");
    }

    printf("check memory:%d\n\r", my_mem_perused(0));
    cJSON_Delete(cjson_test);
    cJSON_Delete(cjson_name);
    cJSON_Delete(cjson_age);
    cJSON_Delete(cjson_name);
    cJSON_Delete(cjson_age);
    cJSON_Delete(cjson_weight);
    cJSON_Delete(cjson_address);
    cJSON_Delete(cjson_address_country);
    cJSON_Delete(cjson_address_zipcode);
    cJSON_Delete(cjson_skill);
    cJSON_Delete(cjson_student);
    printf("freem memory:%d\n\r", my_mem_perused(0));

    while (1);
}
