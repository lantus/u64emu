
void dasmLookupFunction(char *name,DWORD address)
{
	for(DWORD i=0;i<dasmNumLookups;i++)
	{
		if((dasmLookupAddy[i]&0x7fffff)==(address&0x7fffff))
		{
			strcpy(name,dasmLookupName[i]);
			return;
		}
	}
	strcpy(name,"");
}

void dasmLookupIO(char *name,DWORD VAddr)
{
	if((VAddr&0xa0000000)==0xa0000000)
	{
		if((VAddr&0x0f000000)==0x04000000)
		{
			switch(VAddr&0x00f00000)
			{
			case 0x000000:	//sp
				{
					switch((VAddr&0x1f)/4)
					{
					case 0:
						{
							strcpy(name,"SP_MEM_ADDR");
							break;
						}
					case 1:
						{
							strcpy(name,"SP_DRAM_ADDR");
							break;
						}
					case 2:
						{
							strcpy(name,"SP_RD_LEN");
							break;
						}
					case 3:
						{
							strcpy(name,"SP_WR_LEN");
							break;
						}
					case 4:
						{
							strcpy(name,"SP_STATUS");
							break;
						}
					case 5:
						{
							strcpy(name,"SP_DMA_FULL");
							break;
						}
					case 6:
						{
							strcpy(name,"SP_DMA_BUSY");
							break;
						}
					case 7:
						{
							strcpy(name,"SP_SEMAPHORE");
							break;
						}
					}
					break;
				}
			case 0x100000:	//dpc
				{
					switch((VAddr&0x1f)/4)
					{
					case 0:
						{
							strcpy(name,"DPC_START");
							break;
						}
					case 1:
						{
							strcpy(name,"DPC_END");
							break;
						}
					case 2:
						{
							strcpy(name,"DPC_CURRENT");
							break;
						}
					case 3:
						{
							strcpy(name,"DPC_STATUS");
							break;
						}
					case 4:
						{
							strcpy(name,"DPC_CLOCK");
							break;
						}
					case 5:
						{
							strcpy(name,"DPC_BUFBUSY");
							break;
						}
					case 6:
						{
							strcpy(name,"DPC_PIPEBUSY");
							break;
						}
					case 7:
						{
							strcpy(name,"DPC_TMEM");
							break;
						}
					}
					break;
				}
			case 0x200000:	//dps
				{
					strcpy(name,"DPSpan");
					break;
				}
			case 0x300000:	//mi
				{
					switch((VAddr&0xf)/4)
					{
					case 0:
						{
							strcpy(name,"MI_MODE");
							break;
						}
					case 1:
						{
							strcpy(name,"MI_VERSION");
							break;
						}
					case 2:
						{
							strcpy(name,"MI_INTR");
							break;
						}
					case 3:
						{
							strcpy(name,"MI_INTR_MASK");
							break;
						}
					}
					break;
				}
			case 0x400000:	//vi
				{
					switch((VAddr&0x3f)/4)
					{
					case 0:
						{
							strcpy(name,"VI_CONTROL");
							break;
						}
					case 1:
						{
							strcpy(name,"VI_ORIGIN");
							break;
						}
					case 2:
						{
							strcpy(name,"VI_WIDTH");
							break;
						}
					case 3:
						{
							strcpy(name,"VI_INTR");
							break;
						}
					case 4:
						{
							strcpy(name,"VI_CURRENT");
							break;
						}
					case 5:
						{
							strcpy(name,"VI_BURST");
							break;
						}
					case 6:
						{
							strcpy(name,"VI_V_SYNC");
							break;
						}
					case 7:
						{
							strcpy(name,"VI_H_SYNC");
							break;
						}
					case 8:
						{
							strcpy(name,"VI_LEAP");
							break;
						}
					case 9:
						{
							strcpy(name,"VI_H_START");
							break;
						}
					case 10:
						{
							strcpy(name,"VI_V_START");
							break;
						}
					case 11:
						{
							strcpy(name,"VI_V_BURST");
							break;
						}
					case 12:
						{
							strcpy(name,"VI_X_SCALE");
							break;
						}
					case 13:
						{
							strcpy(name,"VI_Y_SCALE");
							break;
						}
					}
					break;
				}
			case 0x500000:	//ai
				{
					switch((VAddr&0x1f)/4)
					{
					case 0:
						{
							strcpy(name,"AI_DRAM_ADDR");
							break;
						}
					case 1:
						{
							strcpy(name,"AI_LEN");
							break;
						}
					case 2:
						{
							strcpy(name,"AI_CONTROL");
							break;
						}
					case 3:
						{
							strcpy(name,"AI_STATUS");
							break;
						}
					case 4:
						{
							strcpy(name,"AI_DACRATE");
							break;
						}
					case 5:
						{
							strcpy(name,"AI_BITRATE");
							break;
						}
					case 6:
						{
							strcpy(name,"AI_UNUSED");
							break;
						}
					case 7:
						{
							strcpy(name,"AI_UNUSED");
							break;
						}
					}
					break;
				}
			case 0x600000:	//pi
				{
					switch((VAddr&0x1f)/4)
					{
					case 0:
						{
							strcpy(name,"PI_DRAM_ADDR");
							break;
						}
					case 1:
						{
							strcpy(name,"PI_CART_ADDR");
							break;
						}
					case 2:
						{
							strcpy(name,"PI_RD_LEN");
							break;
						}
					case 3:
						{
							strcpy(name,"PI_WR_LEN");
							break;
						}
					case 4:
						{
							strcpy(name,"PI_STATUS");
							break;
						}
					case 5:
						{
							strcpy(name,"PI_DOMAIN1");
							break;
						}
					default:
						{
							strcpy(name,"PI_OTHER");
							break;
						}
					}
					break;
				}
			case 0x700000:	//ri
				{
					strcpy(name,"RI_OTHER");
					break;
				}
			case 0x800000:	//si
				{
					switch((VAddr&0x1f)/4)
					{
					case 0:
						{
							strcpy(name,"SI_DRAM_ADDR");
							break;
						}
					case 1:
						{
							strcpy(name,"SI_PIF_ADDR_RD");
							break;
						}
					case 2:
						{
							strcpy(name,"SI_PIF_ADDR_WR");
							break;
						}
					case 3:
						{
							strcpy(name,"SI_STATUS");
							break;
						}
					default:
						{
							strcpy(name,"SI_OTHER");
							break;
						}
					}
					break;
				}
			}
		}
	}
}
