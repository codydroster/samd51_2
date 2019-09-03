GCLK_GENCTRL_Type genctrl_1 =
{
		.bit.DIV = 24,
		.bit.RUNSTDBY = 0,
		.bit.DIVSEL = 0,
		.bit.OE = 0,
		.bit.IDC = 1,
		.bit.GENEN = 1,
		.bit.SRC = GCLK_GENCTRL_SRC_DFLL_Val,
};

	pGclk->GENCTRL[1] = genctrl_1;


	//FDPLL96M input clock source for reference
	pGclk->PCHCTRL[0].bit.WRTLOCK = 0;
	pGclk->PCHCTRL[0].bit.CHEN = 1;
	pGclk->PCHCTRL[0].bit.GEN = GCLK_PCHCTRL_GEN_GCLK1;


	pOscctrl->Dpll[0].DPLLRATIO.bit.LDRFRAC = 0;
	pOscctrl->Dpll[0].DPLLRATIO.bit.LDR = 95;


	pOscctrl->Dpll[0].DPLLCTRLA.bit.ONDEMAND = 0;
	pOscctrl->Dpll[0].DPLLCTRLA.bit.RUNSTDBY = 0;
	pOscctrl->Dpll[0].DPLLCTRLA.bit.ENABLE = 1;

	GCLK_GENCTRL_Type genctrl_0 =
	{
			.bit.DIV = 2,
			.bit.RUNSTDBY = 0,
			.bit.DIVSEL = 0,
			.bit.OE = 1,
			.bit.OOV = 0,
			.bit.IDC = 1,
			.bit.GENEN = 1,
			.bit.SRC = GCLK_GENCTRL_SRC_DPLL0,
	};

	pGclk->GENCTRL[0] = genctrl_0;
