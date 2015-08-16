int GetRAngle(int dx,int dy,int Angle){
	int ang;
	int sx=abs(dx);
	int sy=abs(dy);
	if(!(dx||dy))return 0;
	if(sx>sy){
		ang=div(dy<<8,sx).quot;
		if(dx<0)ang=1024-ang;
	}else{
		ang=512+div(dx<<8,sy).quot;
		if(dy<0)ang=1792-ang;
	};
	ang=Angle-ang;
	while(ang<0)ang+=2048;
	return ang;
};
int GetLAngle(int dx,int dy,int Angle){
	int ang;
	int sx=abs(dx);
	int sy=abs(dy);
	if(!(dx||dy))return 0;
	if(sx>sy){
		ang=div(dy<<8,sx).quot;
		if(dx<0)ang=1024-ang;
	}else{
		ang=512+div(dx<<8,sy).quot;
		if(dy<0)ang=1792-ang;
	};
	ang-=Angle;
	while(ang<0)ang+=2048;
	return ang;
};
void OneObject::CreatePrePath(int x1,int y1){
	PathAsks++;
	TrMap[y][x]=0;
	int sdx=x1-x;
	int	sdy=y1-y;
	int	Cum=0;
	int Pps=0;
	int sx=(sdx>0)?1:-1;
	int sy=(sdy>0)?1:-1;
	int dx=abs(sdx);
	int dy=abs(sdy);
	int	Mdx=dx;
	int	Mdy=dy;
	int	Mx=x;
	int	My=y;
	int	xx1=x;
	int yy1=y;
	int rx=sx;
	int ry=sy;
	if(dx>dy)ry=0;
	if(dy>dx)rx=0;
	int Angle0=GetLAngle(x1-x,y1-y,0);
	int Angle;
	int ddx,ddy;
	int Lvp=0;
	int Rvp=0;
	bool LvpLast=false;
	bool RvpLast=false;
	if(PathX)free(PathX);
	if(PathY)free(PathY);
	PathX=NULL;
	PathY=NULL;
	NIPoints=0;
	CurIPoint=0;
	//соединяем линией начальную и конечную точки. 
	//Оптимизация только по скорости
	__asm{
		mov		ax,word ptr Mdx
		mov		bx,word ptr Mdy
		xor		edx,edx  //Pps
		xor		ecx,ecx  //Cum
		mov		si,word ptr Mx
		mov		di,word ptr My
		cmp		bx,ax
		jae		Lp5xx
		//dx>dy
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		or		ax,ax
		jz		LoopsEnd
		cmp		sy,0
		jl		Lp3xx
		cmp		sx,0
		jl		Lp2begin
		//dx>dy,sx>0,sy>0
Lp1begin:
		inc		si	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp1_1
		sub		cx,word ptr Mdx
		inc		di  //y++
Lp1_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp1begin
		jmp		LoopsEnd
Lp2begin: //dx>dy,sx<0,sy>0
		dec		si	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp2_1
		sub		cx,word ptr Mdx
		inc		di //y++
Lp2_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp2begin
		jmp		LoopsEnd
Lp3xx:	//dy<0
		cmp		sx,0
		jl		Lp4begin
Lp3begin: //dx>dy,sx>0,sy<0
		inc		si	//x++
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp3_1
		sub		cx,word ptr Mdx
		dec		di //y--
Lp3_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp3begin
		jmp		LoopsEnd
Lp4begin: //dx>dy,sx<0,sy<0
		dec		si	//x--
		add		cx,bx
		cmp		cx,word ptr Mdx
		jb		Lp4_1
		sub		cx,word ptr Mdx
		dec		di //y--
Lp4_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		ax
		jnz		Lp4begin
		jmp		LoopsEnd
Lp5xx:	//dx<dy
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		or		bx,bx
		jz		LoopsEnd
		cmp		sx,0
		jl		Lp7xx
		cmp		sy,0
		jl		Lp6begin
Lp5Begin:
		inc		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp5_1
		sub		cx,word ptr dy
		inc		si	//x++
Lp5_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp5begin
		jmp		LoopsEnd
Lp6Begin://sx>0,sy<0
		dec		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp6_1
		sub		cx,word ptr dy
		inc		si	//x++
Lp6_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp6begin
		jmp		LoopsEnd
Lp7xx:	//dx<0
		cmp		sy,0
		jl		Lp8begin
Lp7Begin://dx<0,dy>0
		inc		di	//y++
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp7_1
		sub		cx,word ptr dy
		dec		si	//x--
Lp7_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp7begin
		jmp		LoopsEnd
Lp8Begin://dx<0,dy<0
		dec		di	//y--
		add		cx,ax
		cmp		cx,word ptr dy
		jb		Lp8_1
		sub		cx,word ptr dy
		dec		si	//x--
Lp8_1:
		mov		word ptr[xx+edx],si
		mov		word ptr[yy+edx],di
		inc		edx
		dec		bx
		jnz		Lp8begin
loopsEnd:
		//shr		edx,1
		mov		Pps,edx
	};
		Pps--;
/*this is a place for new code*/
	//Обходим справа
	bool RightPrefer=true;
	int Rtx;//current point 
	int Rty;
	int Ltx;
	int Lty;
	byte Rpx[MaxP];//right path
	byte Rpy[MaxP];
	byte Lpx[MaxP];//left path
	byte Lpy[MaxP];
	int Rpp=1;//index of current point
	bool LDoing;//=true if last point reached
	bool RDoing;
	byte Rdirc;//currend direction
	byte Ldirc;
	int Rmaxalt;//maximum alteration,right path
	int Lmaxalt;//maximum alteration,left path
	int Rppm=0;
	int Lppm=0;
	int Rcum=0;
	int Rcum1=0;
	int Lcum=0;
	int Lcum1=0;
	byte Trr=TrMap[y][x];
	TrMap[y][x]=0;
	//Идем, пока не упремся в стенку
	__asm{
		xor		ebx,ebx
		mov		ecx,Pps
		xor		edx,edx //Rpp
		mov		al,[xx+edx]
		mov		[Rpx+edx],al
		mov		[Lpx+edx],al
		mov		al,[yy+edx]
		mov		[Rpy+edx],al
		mov		[Lpy+edx],al
		inc		edx
uuu_Loop:
		mov		al,[xx+edx]
		mov		[Rpx+edx],al
		mov		[Lpx+edx],al
		mov		bl,al
		mov		al,[yy+edx]
		mov		[Rpy+edx],al
		mov		[Lpy+edx],al
		mov		bh,al
//		add		bx,0101h
		cmp		byte ptr[TrMap+ebx],0
		jnz		uuu_end
		inc		edx
		loop	uuu_Loop		
uuu_end:
		//shr		edx,1
		mov		Rpp,edx
	};
	Rtx=xx[Rpp-1];
	Rty=yy[Rpp-1];
	
	// Если dx>dy,то на каждом шагу dx изменяетя строго на 1
	if(Rtx!=x1||Rty!=y1){
		//LLock[y][x]=false;
		Rpp-=1;
		Rtx=xx[Rpp];
		Rty=yy[Rpp];
		Ltx=xx[Rpp];
		Lty=yy[Rpp];
		int Ppi=Rpp+1;
		LDoing=true;
		RDoing=true;
		//Ищем, пока находимся в занятой зоне
		while(TrMap[yy[Ppi]][xx[Ppi]]&&Ppi<Pps)Ppi++;
		if(Ppi>Pps)LDoing=false;//Кон.точка недостижима
		int Xls=xx[Ppi-1];
		int Yls=yy[Ppi-1];
		//Уперлись...Вычисляем направление движения
		Rdirc=drr[(xx[Rpp+1]-xx[Rpp]+1)*3+yy[Rpp+1]-yy[Rpp]+1];
		Ldirc=Rdirc;
		//Выбираем начальное направление-right
		FillCirc(Rtx,Rty);
		int dirc1=(Rdirc+1)&7;
		for(int z=0;circ[dirc1]&&z<7;dirc1++,z++);
		Rdirc=dirc1&7;
		//-left
		dirc1=8+((Ldirc+7)&7);
		for(z=0;circ[dirc1]&&z<7;dirc1--,z++);
		Ldirc=dirc1&7;
		//Идем по правому краю до тех пор пока вновь не пересе-
		//чемся с прямой линией, соединяющей начальную и конечную
		//точки
		Rmaxalt=0;
		Lmaxalt=0;
		while(Rpp<MaxP-8&&LDoing&&RDoing){
			//пытаемся повернуть направо
			FillCirc(Rtx,Rty);
			int dirc1=(Rdirc+7)&7;
			for(int z=0;z<6&&circ[dirc1];dirc1++,z++);
			Rdirc=dirc1&7;
			Rpp++;
			int Tdx=idrx[Rdirc];
			int Tdy=idry[Rdirc];
			Rcum-=sdy*Tdx;
			Rcum+=sdx*Tdy;
			Rtx+=Tdx;
			Rty+=Tdy;
			Rpx[Rpp]=Rtx;Rpy[Rpp]=Rty;
			Angle=GetLAngle(Rtx-x,Rty-y,Angle0);
			if(Angle>Rmaxalt){
				Rmaxalt=Angle;
				Rppm=Rpp;
			};
			//проверяем условие прямой видимости
			ddx=Rtx-x;
			ddy=Rty-y;
			if(ddx>1)ddx=1;
			if(ddx<-1)ddx=-1;
			if(ddy>1)ddy=1;
			if(ddy<-1)ddy=-1;
			if(!TrMap[Rty+ddy][Rtx+ddx]){
				if(!RvpLast){
					Rvp=Rpp;
					RvpLast=true;
				};
			}else RvpLast=false;
			//the same, but left
			FillCirc(Ltx,Lty);
			dirc1=8+((Ldirc+1)&7);
			for(z=0;z<6&&circ[dirc1];dirc1--,z++);
			Ldirc=dirc1&7;
			Tdx=idrx[Ldirc];
			Tdy=idry[Ldirc];
			Lcum+=sdy*Tdx;
			Lcum-=sdx*Tdy;
			Ltx+=Tdx;
			Lty+=Tdy;
			Lpx[Rpp]=Ltx;Lpy[Rpp]=Lty;
			Angle=GetRAngle(Rtx-x,Rty-y,Angle0);
			if(Angle>Lmaxalt){
				Lmaxalt=Angle;
				Lppm=Rpp;
			};
			//проверяем условие прямой видимости
			ddx=Ltx-x;
			ddy=Lty-y;
			if(ddx>1)ddx=1;
			if(ddx<-1)ddx=-1;
			if(ddy>1)ddy=1;
			if(ddy<-1)ddy=-1;
			if(!TrMap[Lty+ddy][Ltx+ddx]){
				if(!LvpLast){
					Lvp=Rpp;
					LvpLast=true;
				};
			}else RvpLast=false;
			//Вычисляем значение y на прямой линии, соотв. 
			//данному x
			if(Rcum<=0&&Rcum1>=0){
				if(dx>dy){
					if(sx>0){
						if(Rtx>=Xls)RDoing=false;
					}else if(Rtx<=Xls)RDoing=false;
				}else{
					if(sy>0){
						if(Rty>=Yls)RDoing=false;
					}else if(Rty<=Yls)RDoing=false;
				};
				if(!RDoing)RightPrefer=true;
			};
			Rcum1=Rcum;
			//the same for left
			if(Lcum<=0&&Lcum1>=0){
				if(dx>dy){
					if(sx>0){
						if(Ltx>=Xls)LDoing=false;
					}else if(Ltx<=Xls)LDoing=false;
				}else{
					if(sy>0){
						if(Lty>=Yls)LDoing=false;
					}else if(Lty<=Yls)LDoing=false;
				};
				if(!LDoing)RightPrefer=false;
			};
			Lcum1=Lcum;
		};
		//LLock[y][x]=true;
		if(Rpp<MaxP-9){
			if(RightPrefer){
				if(Rppm+1<Rpp)Rppm+=1;else Rppm=Rpp;
				//memcpy(xx,Rpx,(Rppm+1));
				//memcpy(yy,Rpy,(Rppm+1));
				Pps=Rppm;
			}else{
				if(Lppm+1<Rpp)Lppm+=1;else Lppm=Rpp;
				//memcpy(xx,Lpx,(Lppm+1));
				//memcpy(yy,Lpy,(Lppm+1));
				Pps=Lppm;
				Rvp=Lvp;
				if(Rvp<Pps)Rvp=Pps;
				
			};
			PathX=new byte [Rvp-Pps+1];
			PathY=new byte [Rvp-Pps+1];
			memcpy(PathX,&Rpx[Pps],Rvp-Pps+1);
			memcpy(PathX,&Rpx[Pps],Rvp-Pps+1);
			NIPoints=Rvp-Pps+1;
			CurIPoint=0;
		}else return;
		//Оптимизация пути
		//Pps=Optima1(Pps);
	};
	TrMap[y][x]=Trr;
	NowBuf=GetAsmBlock();
	if(!int(NowBuf))return;
	Ofst=0;
	memcpy(&NowBuf[OneAsmSize-4],&Ofst,4);
	InLineCom=NowBuf;
	LineOffset=0;
	cmLoadAnm(AnmGoKind,1,0);
	//cmLoadAnm(1,1,0);
	cmLoadAnm(AnmStandKind,0,0);
	for(int i=1;i<=Pps;i++){
		//byte (*XXX)[64];
		//memcpy(&XXX,&NowBuf,4);
		//cmSetDir(xx[i]-xx[i-1],yy[i]-yy[i-1]);
		if(i==1)cmPerfAnm(0);//-------??????????????????????
		cmSetXY(xx[i]-xx[i-1],yy[i]-yy[i-1]);
		//cmPerfAnm(1);
		//cmSetXYDirX(xx[i],yy[i],xx[i]-xx[i-1],yy[i]-yy[i-1],1);
		//cmPerfAnm(1);
	};
	cmLoadAnm(AnmStandKind,0,0);
	cmPerfAnm(0);
	//cmDone();
	cmRet();
};