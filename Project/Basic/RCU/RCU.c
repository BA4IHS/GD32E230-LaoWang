#include "RCU.h"

#define RCU_MODIFY(__delay)     do{                                     \
                                    volatile uint32_t i,reg;            \
                                    if(0 != __delay){                   \
                                        reg = RCU_CFG0;                 \
                                        reg &= ~(RCU_CFG0_AHBPSC);      \
                                        /* CK_AHB = SYSCLK/2 */         \
                                        reg |= RCU_AHB_CKSYS_DIV2;      \
                                        RCU_CFG0 = reg;                 \
                                        for(i=0; i<__delay; i++){       \
                                        }                               \
                                        reg = RCU_CFG0;                 \
                                        reg &= ~(RCU_CFG0_AHBPSC);      \
                                        reg |= RCU_AHB_CKSYS_DIV4;      \
                                        /* CK_AHB = SYSCLK/4 */         \
                                        RCU_CFG0 = reg;                 \
                                        for(i=0; i<__delay; i++){       \
                                        }                               \
                                    }                                   \
                                }while(0)

#define RCU_MODIFY_UP_2(__delay)     do{                                    \
                                        volatile uint32_t i,reg;            \
                                        if(0 != __delay){                   \
                                            for(i=0; i<__delay; i++){       \
                                            }                               \
                                            reg = RCU_CFG0;                 \
                                            reg &= ~(RCU_CFG0_AHBPSC);      \
                                            reg |= RCU_AHB_CKSYS_DIV2;      \
                                            RCU_CFG0 = reg;                 \
                                            for(i=0; i<__delay; i++){       \
                                            }                               \
                                            reg = RCU_CFG0;                 \
                                            reg &= ~(RCU_CFG0_AHBPSC);      \
                                            reg |= RCU_AHB_CKSYS_DIV1;      \
                                            RCU_CFG0 = reg;                 \
                                    }                                       \
                                }while(0)

static void _soft_delay_(uint32_t time)
{
    __IO uint32_t i;
    for(i=0; i<time*10; i++){
    }
}


void RCU_Init(void)
{
	    uint32_t timeout = 0U;
    uint32_t stab_flag = 0U;
    __IO uint32_t reg_temp;
    
    RCU_MODIFY(0x80);
    /* select IRC8M as system clock source, deinitialize the RCU */
    rcu_system_clock_source_config(RCU_CKSYSSRC_IRC8M);
    _soft_delay_(100);
    rcu_deinit();
    
    /* enable IRC8M */
    RCU_CTL0 |= RCU_CTL0_IRC8MEN;

    /* wait until IRC8M is stable or the startup time is longer than IRC8M_STARTUP_TIMEOUT */
    do{
        timeout++;
        stab_flag = (RCU_CTL0 & RCU_CTL0_IRC8MSTB);
    }
    while((0U == stab_flag) && (IRC8M_STARTUP_TIMEOUT != timeout));

    /* if fail */
    if(0U == (RCU_CTL0 & RCU_CTL0_IRC8MSTB)){
        while(1){
        }
    }

    FMC_WS = (FMC_WS & (~FMC_WS_WSCNT)) | WS_WSCNT_2;
    
    /* AHB = SYSCLK/4 */
    RCU_CFG0 |= RCU_AHB_CKSYS_DIV4;
    /* APB2 = AHB */
    RCU_CFG0 |= RCU_APB2_CKAHB_DIV1;
    /* APB1 = AHB */
    RCU_CFG0 |= RCU_APB1_CKAHB_DIV1;
    /* PLL = (IRC8M/2) * 18 = 72 MHz */
    RCU_CFG0 &= ~(RCU_CFG0_PLLSEL | RCU_CFG0_PLLMF);
    RCU_CFG0 |= (RCU_PLLSRC_IRC8M_DIV2 | RCU_PLL_MUL18);

    /* enable PLL */
    RCU_CTL0 |= RCU_CTL0_PLLEN;

    /* wait until PLL is stable */
    while(0U == (RCU_CTL0 & RCU_CTL0_PLLSTB)){
    }

    reg_temp = RCU_CFG0;
    /* select PLL as system clock */
    reg_temp &= ~RCU_CFG0_SCS;
    reg_temp |= RCU_CKSYSSRC_PLL;
    RCU_CFG0 = reg_temp;

    /* wait until PLL is selected as system clock */
    while(0U == (RCU_CFG0 & RCU_SCSS_PLL)){
    }
    RCU_MODIFY_UP_2(0x80);
}



