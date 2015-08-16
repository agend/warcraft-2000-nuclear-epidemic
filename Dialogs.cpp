#include "ddini.h"
#include "ResFile.h"
#include "fastdraw.h"
#include "mapdiscr.h"
#include "mouse.h"
#include "Dialogs.h"
#include "fonts.h"
#include <assert.h>
extern bool realLpressed;
extern bool KeyPressed;
extern int LastKey;
extern bool MUSTDRAW;
extern int RealLx;
extern int RealLy;
void ShowString(int x,int y,LPCSTR lps,lpRLCFont lpf);
DialogsSystem::DialogsSystem(int x,int y){
	BaseX=x;
	BaseY=y;
	for(int i=0;i<512;i++)DSS[i]=NULL;
	NDial=0;
	Active=NULL;
	Passive=NULL;
	Disabled=NULL;
	Message=NULL;
};
void DialogsSystem::SetFonts(RLCFont* fActive,
							 RLCFont* fPassive,
							 RLCFont* fDisabled,
							 RLCFont* fMessage){
	Active=fActive;
	Passive=fPassive;
	Disabled=fDisabled;
	Message=fMessage;
};
//-----------------class Picture
bool Picture_OnMouseOver(SimpleDialog* SD){
	Picture* Pic=(Picture*)SD;
	if(Pic->ActivePicture!=Pic->PassivePicture)
		SD->NeedToDraw=true;
	return false;
};
bool Picture_OnDraw(SimpleDialog* SD){
	if(!SD)return false;
	Picture* pic=(Picture*)SD;
	if(!pic->Enabled&&pic->DisabledPicture){
		pic->DisabledPicture->Draw(pic->x,pic->y);
		return true;
	};
	if(pic->MouseOver&&pic->ActivePicture){
		pic->ActivePicture->Draw(pic->x,pic->y);
		return true;
	};
	if(pic->PassivePicture){
		pic->PassivePicture->Draw(pic->x,pic->y);
		return true;
	};
	return false;
};
Picture* DialogsSystem::addPicture(SimpleDialog* Parent,
						int px,int py,
						SQPicture* Active,
						SQPicture* Passive,
						SQPicture* Disabled){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		Picture* pic=new Picture;
		DSS[i]=pic;
		if(Active!=Passive)pic->MouseOverActive=true;
		pic->NeedToDraw=true;
		pic->Parent=Parent;
		pic->x=BaseX+px;
		pic->y=BaseY+py;
		pic->x1=pic->x+Passive->PicPtr[0]-1;
		pic->y1=pic->y+Passive->PicPtr[1]-1;
		pic->ActivePicture=Active;
		pic->PassivePicture=Passive;
		pic->DisabledPicture=Disabled;
		pic->OnDraw=&Picture_OnDraw;
		pic->OnMouseOver=&Picture_OnMouseOver;
		pic->Enabled=true;
		pic->Active=false;
		return pic;
	};
	return NULL;
};
//-----------------class RLCPicture
bool RLCPicture_OnDraw(SimpleDialog* SD){
	if(!SD)return false;
	RLCPicture* pic=(RLCPicture*)SD;
	if(!pic->Enabled&&pic->DisabledPicture){
		ShowRLCItem(pic->x,pic->y,pic->DisabledPicture,pic->dpic,0);
		return true;
	};
	if(pic->MouseOver&&pic->ActivePicture){
		ShowRLCItem(pic->x,pic->y,pic->ActivePicture,pic->apic,0);
		return true;
	};
	if(pic->PassivePicture){
		ShowRLCItem(pic->x,pic->y,pic->PassivePicture,pic->dpic,0);
		return true;
	};
	return false;
};
RLCPicture* DialogsSystem::addRLCPicture(SimpleDialog* Parent,int x,int y,
						RLCTable* Active,byte apic,
						RLCTable* Passive,byte ppic,
						RLCTable* Disabled,byte dpic){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		RLCPicture* pic=new RLCPicture;
		DSS[i]=pic;
		if(Active!=Passive)pic->MouseOverActive=true;
		pic->NeedToDraw=true;
		pic->Parent=Parent;
		pic->x=BaseX+x;
		pic->y=BaseY+y;
		pic->x1=pic->x+GetRLCWidth(*Passive,ppic);
		pic->y1=pic->y+GetRLCHeight(*Passive,ppic);
		pic->ActivePicture=Active;
		pic->apic=apic;
		pic->PassivePicture=Passive;
		pic->ppic=ppic;
		pic->DisabledPicture=Disabled;
		pic->dpic=dpic;
		pic->OnDraw=&Picture_OnDraw;
		pic->OnMouseOver=&Picture_OnMouseOver;
		pic->Enabled=true;
		pic->Active=false;
		return pic;
	};
	return NULL;
};
//------------end of the class Picture


//------------class TextButton
int GetRLen(char* s,RLCFont* font){
	if(!int(s))return 0;
	int x=0;
	for(int i=0;s[i]!=0;i++)
		x+=GetRLCWidth(font->RLC,font->FirstSymbol+s[i]);
	return x;
};
void DrawString(int x,int y,char* str,RLCFont* fnt,byte Align){
	int xx;
	if(!str)return;
	switch(Align){
	case 0:	ShowString(x,y,str,fnt);
			return;
	case 1: //center align
			xx=x-(GetRLen(str,fnt)>>1);
			ShowString(xx,y,str,fnt);
			return;
	case 2: //right align
			xx=x-GetRLen(str,fnt);
			ShowString(xx,y,str,fnt);
	};
};
bool TextButton_OnDraw(SimpleDialog* SD){
	if(!SD)return false;
	TextButton* tb=(TextButton*)SD;
	RLCFont* CFN=NULL;
	if(!tb->Enabled&&tb->DisabledFont)CFN=tb->DisabledFont;
	else 
	if(tb->MouseOver&&tb->ActiveFont)CFN=tb->ActiveFont;
	else
	if(tb->PassiveFont)CFN=tb->PassiveFont;
	if(CFN){
		int rlen=GetRLen(tb->Message,CFN);
		switch(tb->Align){
		case 1:tb->x=tb->xc-(rlen>>1);
			break;
		case 2:tb->x=tb->yc-rlen;
		};
		tb->x1=tb->x+rlen-1;
		tb->y1=tb->y+GetRLCHeight(CFN->RLC,'W');
		ShowString(tb->x,tb->y,tb->Message,CFN);
		return true;
	};
	return false;
};
bool TextButton_OnMouseOver(SimpleDialog* SD){
	TextButton* tb=(TextButton*)SD;
	if(tb->PassiveFont!=tb->ActiveFont)
		SD->NeedToDraw=true;
	return false;
};
bool TextButton_Destroy(SimpleDialog* SD){
	TextButton* tb=(TextButton*)SD;
	if(tb->Message)free(tb->Message);
	return true;
};

TextButton* DialogsSystem::addTextButton(SimpleDialog* Parent,
						int px,int py,char* str,
						RLCFont* Active,
						RLCFont* Passive,
						RLCFont* Disabled,
						byte Align){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		TextButton* tb=new TextButton;
		DSS[i]=tb;
		tb->MouseOverActive=true;
		tb->NeedToDraw=true;
		tb->Parent=Parent;
		tb->x=BaseX+px;
		tb->y=BaseY+py;
		tb->xc=BaseX+px;
		tb->yc=BaseY+py;
		int rlen=GetRLen(str,Passive);
		switch(Align){
		case 1:tb->x-=rlen>>1;
			break;
		case 2:tb->x-=rlen;
		};
		tb->x1=tb->x+rlen-1;
		tb->y1=tb->y+GetRLCHeight(Passive->RLC,'W');
		tb->ActiveFont=Active;
		tb->PassiveFont=Passive;
		tb->DisabledFont=Disabled;
		tb->ActiveDX=0;
		tb->ActiveDY=0;
		tb->Align=Align;
		tb->Message=new char[strlen(str)+1];
		strcpy(tb->Message,str);
		tb->OnDraw=&TextButton_OnDraw;
		tb->OnMouseOver=&TextButton_OnMouseOver;
		tb->Destroy=&TextButton_Destroy;
		tb->Enabled=true;
		tb->Active=false;
		return tb;
	};
	return NULL;
};
//------------end of the class TextButton
//-------class : button with text-----------
bool BpxTextButton_OnDraw(SimpleDialog* SD){
	if(!SD)return false;
	BpxTextButton* tb=(BpxTextButton*)SD;
	//drawing button
	SQPicture* SP;
	tb->xc=(tb->x+tb->x1)>>1;
	tb->yc=(tb->y+tb->y1)>>1;
	if(tb->Enabled){
		if(tb->MouseOver&&realLpressed)SP=tb->ActivePicture;
		else SP=tb->PassivePicture;
	}else SP=tb->DisabledPicture;
	SP->Draw(tb->x,tb->y);
	//drawing text
	RLCFont* CFN=NULL;
	if(!tb->Enabled&&tb->DisabledFont)CFN=tb->DisabledFont;
	else 
	if(tb->MouseOver&&tb->ActiveFont)CFN=tb->ActiveFont;
	else
	if(tb->PassiveFont)CFN=tb->PassiveFont;
	if(CFN){
		int rlen=GetRLen(tb->Message,CFN);
		ShowString(tb->xc-(rlen>>1),tb->yc-(GetRLCHeight(CFN->RLC,'W')>>1),tb->Message,CFN);
		return true;
	};
	return false;
};
bool BpxTextButton_OnMouseOver(SimpleDialog* SD){
	if(SD->MouseOver)SD->NeedToDraw=true;
	return false;
};
BpxTextButton* DialogsSystem::addBpxTextButton(SimpleDialog* Parent,
						int px,int py,char* str,
						RLCFont* Active,
						RLCFont* Passive,
						RLCFont* Disabled,
						SQPicture* pActive,
						SQPicture* pPassive,
						SQPicture* pDisabled){
for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		BpxTextButton* tb=new BpxTextButton;
		tb->MouseOverActive=true;
		tb->NeedToDraw=true;
		tb->Parent=Parent;
		tb->x=BaseX+px;
		tb->y=BaseY+py;
		tb->x1=tb->x+pPassive->PicPtr[0]-1;
		tb->y1=tb->y+pPassive->PicPtr[1]-1;
		tb->ActivePicture=pActive;
		tb->PassivePicture=pPassive;
		tb->DisabledPicture=pDisabled;
		DSS[i]=tb;
		tb->ActiveFont=Active;
		tb->PassiveFont=Passive;
		tb->DisabledFont=Disabled;
		tb->ActiveDX=0;
		tb->ActiveDY=0;
		tb->Message=new char[strlen(str)+1];
		strcpy(tb->Message,str);
		tb->OnDraw=&BpxTextButton_OnDraw;
		tb->Destroy=&TextButton_Destroy;
		tb->Enabled=true;
		tb->Active=false;
		tb->OnMouseOver=&BpxTextButton_OnMouseOver;
		return tb;
	};
	return NULL;
};
//--------end of button with text----------//
//------class : Vertical scroll bar--------//
bool MouseIns(int x,int y,int x1,int y1){
	if(mouseX>=x&&mouseY>=y&&mouseX<=x1&&mouseY<=y1)return true;
	else return false;
};
bool MouseInsL(int x,int y,int lx,int ly){
	return MouseIns(x,y,x+lx-1,y+ly-1);
};
bool VScrollBar_Draw(SimpleDialog* SD){
	VScrollBar* tb=(VScrollBar*)SD;
	if(tb->GetMaxPos()>0){
		tb->sbar0->Draw(tb->x,tb->sby+2);
		if(MouseInsL(tb->x,tb->y,tb->sblx,tb->btnly)&&realLpressed)
			tb->btn_up1p->DrawTransparent(tb->x,tb->y);
		else tb->btn_up1->DrawTransparent(tb->x,tb->y);
		if(MouseInsL(tb->x,tb->sby+tb->sbly,tb->sblx,tb->btnly)&&realLpressed)
			tb->btn_dn1p->DrawTransparent(tb->x,tb->sby+tb->sbly);
		else tb->btn_dn1->DrawTransparent(tb->x,tb->sby+tb->sbly);
		tb->marker->DrawTransparent(tb->x+1,tb->y+tb->btnly+div(int(tb->sbly-tb->mkly+2)*tb->SPos,tb->SMaxPos).quot);
	}else{
		tb->sbar1->Draw(tb->x,tb->sby+2);
		tb->btn_up0->DrawTransparent(tb->x,tb->y);
		tb->btn_dn0->DrawTransparent(tb->x,tb->sby+tb->sbly);
	};
	return true;
};
bool VScrollBar_MouseOver(SimpleDialog* SD){
	VScrollBar* tb=(VScrollBar*)SD;
	int dt=GetTickCount()-tb->LastTime;
	if(tb->SMaxPos>0){
		if(!realLpressed)tb->Zaxvat=false;
		int my=tb->y+tb->btnly+div(int(tb->sbly-tb->mkly+2)*tb->SPos,tb->SMaxPos).quot;
		if(!tb->Zaxvat){
			if(MouseInsL(tb->x,tb->y,tb->sblx,tb->btnly)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				int txx=tb->SPos;
				if(tb->SPos>0)tb->SPos--;
				if(tb->SPos!=txx)tb->NeedToDraw=true;
			};
			if(MouseInsL(tb->x,tb->sby+tb->sbly,tb->sblx,tb->btnly)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				int txx=tb->SPos;
				if(tb->SPos<tb->SMaxPos)tb->SPos++;
				if(tb->SPos!=txx)tb->NeedToDraw=true;
			};
			if(MouseIns(tb->x,tb->y+tb->btnly,tb->x+tb->sblx-1,my)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				int txx=tb->SPos;
				if(tb->SPos>6)tb->SPos-=6;
				else tb->SPos=0;
				if(tb->SPos!=txx)tb->NeedToDraw=true;
			};
			if(MouseIns(tb->x,my+tb->mkly,tb->x+tb->sblx-1,tb->y+tb->sbly+tb->btnly-1)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				int txx=tb->SPos;
				if(tb->SMaxPos-tb->SPos>6)tb->SPos+=6;
				else tb->SPos=tb->SMaxPos;
				if(tb->SPos!=txx)tb->NeedToDraw=true;
			};
			if(MouseInsL(tb->x,my,tb->sblx,tb->mkly)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				tb->Zaxvat=true;
			};
		}else{
			int txx=tb->SPos;
			tb->SPos=div((mouseY-tb->sby-(tb->mkly>>1))*tb->SMaxPos,tb->sbly-tb->mkly).quot;
			if(tb->SPos<0)tb->SPos=0;
			if(tb->SPos>tb->SMaxPos)tb->SPos=tb->SMaxPos;
			if(tb->SPos!=txx)tb->NeedToDraw=true;
		};
	};
	return true;
};
VScrollBar* DialogsSystem::addVScrollBar(SimpleDialog* Parent,
						int x,int y,int MaxPos,int Pos,
						SQPicture* btn_up0,
						SQPicture* btn_up1,
						SQPicture* btn_up1p,
						SQPicture* btn_dn0,
						SQPicture* btn_dn1,
						SQPicture* btn_dn1p,
						SQPicture* sbar0,
						SQPicture* sbar1,
						SQPicture* marker){
				
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		VScrollBar* tb=new VScrollBar;
		DSS[i]=tb;
		tb->NeedToDraw=true;
		tb->Parent=Parent;
		tb->x=BaseX+x;
		tb->y=BaseY+y;
		tb->sblx=sbar0->PicPtr[0];
		tb->sbly=sbar0->PicPtr[1];
		tb->mkly=marker->PicPtr[1];
		tb->btnly=btn_dn1->PicPtr[1]-2;
		tb->y1=tb->y+tb->sbly+(tb->btnly<<1)-1;
		tb->x1=tb->x+tb->sblx-1;
		tb->sby=tb->y+tb->btnly;
		tb->SMaxPos=MaxPos;
		tb->SPos=Pos;
		tb->btn_up0=btn_up0;
		tb->btn_up1=btn_up1;
		tb->btn_up1p=btn_up1p;
		tb->btn_dn0=btn_dn0;
		tb->btn_dn1=btn_dn1;
		tb->btn_dn1p=btn_dn1p;
		tb->sbar0=sbar0;
		tb->sbar1=sbar1;
		tb->marker=marker;
		tb->Zaxvat=false;
		tb->OnDraw=&VScrollBar_Draw;
		tb->OnMouseOver=&VScrollBar_MouseOver;
		tb->Enabled=true;
		tb->Active=false;
		return tb;
	};
	return NULL;
};
//-------end of vertical Scroll bar--------//
//------class : Horizontal scroll bar--------//
bool HScrollBar_Draw(SimpleDialog* SD){
	VScrollBar* tb=(VScrollBar*)SD;
	if(tb->SMaxPos>0){
		tb->sbar0->Draw(tb->sby+2,tb->y);
		//left click
		if(MouseInsL(tb->x,tb->y,tb->btnly,tb->sblx)&&realLpressed)
			tb->btn_up1p->DrawTransparent(tb->x,tb->y);
		else tb->btn_up1->DrawTransparent(tb->x,tb->y);
		//right click
		if(MouseInsL(tb->sby+tb->sbly,tb->y,tb->btnly,tb->sblx)&&realLpressed)
			tb->btn_dn1p->DrawTransparent(tb->sby+tb->sbly,tb->y);
		else tb->btn_dn1->DrawTransparent(tb->sby+tb->sbly,tb->y);
		tb->marker->DrawTransparent(tb->x+tb->btnly+div(int(tb->sbly-tb->mkly+2)*tb->SPos,tb->SMaxPos).quot,tb->y+1);
	}else{
		tb->sbar1->Draw(tb->sby+2,tb->y);
		tb->btn_up0->DrawTransparent(tb->x,tb->y);
		tb->btn_dn0->DrawTransparent(tb->sby+tb->sbly,tb->x);
	};
	return true;
};
bool HScrollBar_MouseOver(SimpleDialog* SD){
	VScrollBar* tb=(VScrollBar*)SD;
	int dt=GetTickCount()-tb->LastTime;
	if(tb->SMaxPos>0){
		if(!realLpressed)tb->Zaxvat=false;
		int my=tb->x+tb->btnly+div(int(tb->sbly-tb->mkly+2)*tb->SPos,tb->SMaxPos).quot;
		if(!tb->Zaxvat){
			if(MouseInsL(tb->x,tb->y,tb->btnly,tb->sblx)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				int txx=tb->SPos;
				if(tb->SPos>0)tb->SPos--;
				if(tb->SPos!=txx)tb->NeedToDraw=true;
			};
			if(MouseInsL(tb->sby+tb->sbly,tb->y,tb->btnly,tb->sblx)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				int txx=tb->SPos;
				if(tb->SPos<tb->SMaxPos)tb->SPos++;
				if(tb->SPos!=txx)tb->NeedToDraw=true;
			};
			if(MouseIns(tb->x+tb->btnly,tb->y,my,tb->y+tb->sblx-1)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				int txx=tb->SPos;
				if(tb->SPos>6)tb->SPos-=6;
				else tb->SPos=0;
				if(tb->SPos!=txx)tb->NeedToDraw=true;
			};
			if(MouseIns(my+tb->mkly,tb->y,tb->x+tb->sbly+tb->btnly-1,tb->y+tb->sblx-1)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				int txx=tb->SPos;
				if(tb->SMaxPos-tb->SPos>6)tb->SPos+=6;
				else tb->SPos=tb->SMaxPos;
				if(tb->SPos!=txx)tb->NeedToDraw=true;
			};
			if(MouseInsL(my,tb->y,tb->mkly,tb->sblx)&&Lpressed&&dt>150){
				tb->LastTime=GetTickCount();
				//Lpressed=false;
				tb->Zaxvat=true;
			};
		}else{
			int txx=tb->SPos;
			tb->SPos=div((mouseX-tb->sby-(tb->mkly>>1))*tb->SMaxPos,tb->sbly-tb->mkly).quot;
			if(tb->SPos<0)tb->SPos=0;
			if(tb->SPos>tb->SMaxPos)tb->SPos=tb->SMaxPos;
			if(tb->SPos!=txx)tb->NeedToDraw=true;
		};
	};
	return true;
};
VScrollBar* DialogsSystem::addHScrollBar(SimpleDialog* Parent,
						int x,int y,int MaxPos,int Pos,
						SQPicture* btn_up0,
						SQPicture* btn_up1,
						SQPicture* btn_up1p,
						SQPicture* btn_dn0,
						SQPicture* btn_dn1,
						SQPicture* btn_dn1p,
						SQPicture* sbar0,
						SQPicture* sbar1,
						SQPicture* marker){
				
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		VScrollBar* tb=new VScrollBar;
		DSS[i]=tb;
		tb->NeedToDraw=true;
		tb->Parent=Parent;
		tb->x=BaseX+x;
		tb->y=BaseY+y;
		tb->sblx=sbar0->PicPtr[1];
		tb->sbly=sbar0->PicPtr[0];
		tb->mkly=marker->PicPtr[0];
		tb->btnly=btn_dn1->PicPtr[0]-2;
		tb->x1=tb->x+tb->sbly+(tb->btnly<<1)-1;
		tb->y1=tb->y+tb->sblx-1;
		tb->sby=tb->x+tb->btnly;
		tb->SMaxPos=MaxPos;
		tb->SPos=Pos;
		tb->btn_up0=btn_up0;
		tb->btn_up1=btn_up1;
		tb->btn_up1p=btn_up1p;
		tb->btn_dn0=btn_dn0;
		tb->btn_dn1=btn_dn1;
		tb->btn_dn1p=btn_dn1p;
		tb->sbar0=sbar0;
		tb->sbar1=sbar1;
		tb->marker=marker;
		tb->Zaxvat=false;
		tb->OnDraw=&HScrollBar_Draw;
		tb->OnMouseOver=&HScrollBar_MouseOver;
		tb->Enabled=true;
		tb->Active=false;
		return tb;
	};
	return NULL;
};
//-------end of horizontal Scroll bar--------//
//-------------class : ListBox---------------//
ListBoxItem* ListBox::GetItem(int i){
	ListBoxItem* LB=FirstItem;
	for(int j=0;j<i;j++){
		if(LB)LB=LB->NextItem;
	};
	return LB;
};
void ListBox::AddItem(char* str,int n){
	NeedToDraw=true;
	ListBoxItem* LB=new ListBoxItem;
	if(LastItem){
		LastItem->NextItem=LB;
		LB->NextItem=NULL;
	}else{
		FirstItem=LB;
		LastItem=LB;
		LB->NextItem=NULL;
	};
	LastItem=LB;
	NItems++;
	LB->Message=new char [strlen(str)+1];
	strcpy(LB->Message,str);
	LB->Param1=n;
};
void ListBox::ClearItems(){
	NeedToDraw=true;
	ListBoxItem* LB1,*LB=FirstItem;
	while(LB){
		free(LB->Message);
		LB1=LB->NextItem;
		free(LB);
		LB=LB1;
	};
	NItems=0;
	FirstItem=NULL;
	LastItem=NULL;
	NItems=0;
	CurItem=0;
	FLItem=0;
};
void ListBox::SetFirstPos(int n){
	if(n!=FLItem)NeedToDraw=true;
	FLItem=n;
};
void ListBox::SetCurrentItem(int n){
	if(n!=CurItem)NeedToDraw=true;
	CurItem=n;
	if(CurItem>=NItems)CurItem=NItems-1;
	if(!NItems)CurItem=-1;
	if(CurItem<FLItem)FLItem=CurItem;
};
bool ListBox_OnDraw(SimpleDialog* SD){
	ListBox* LB=(ListBox*)SD;
	if(LB){
		int y=LB->y;
		for(int i=0;i<LB->ny;i++){
			LB->ItemPic->Draw(LB->x,y);
			int Iind=LB->FLItem+i;
			ListBoxItem* str=LB->GetItem(Iind);
			if(str){
				if(Iind==LB->CurItem)
					ShowString(LB->x+8,y+8,str->Message,LB->AFont);
				else ShowString(LB->x+8,y+8,str->Message,LB->PFont);
			};
			y+=LB->oneLy-1;
		};
	};
	return true;
};
bool ListBox_OnMouseOver(SimpleDialog* SD){
	if(!Lpressed)return false;
	ListBox* LB=(ListBox*)SD;
	SD->NeedToDraw=true;
	int ni=div(mouseY-LB->y,LB->oneLy-1).quot;
	if(ni<LB->ny){
		if(LB->FLItem+ni<LB->NItems)
			LB->CurItem=LB->FLItem+ni;
		else LB->CurItem=LB->NItems-1;
	};
	return true;
};
bool ListBox_Refresh(SimpleDialog* SD){
	ListBox* LB=(ListBox*)SD;
	if(LB->VS){
		VScrollBar* VS=LB->VS;
		VS->SetMaxPos(LB->NItems-LB->ny);
		if(VS->GetMaxPos()>0)LB->FLItem=VS->GetPos();
	};
	return true;
};
ListBox* DialogsSystem::addListBox(SimpleDialog* Parent,
						int x,int y,int Ny,
						SQPicture* ItemPic,
						RLCFont* AFont,
						RLCFont* PFont,
						VScrollBar* VS){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		ListBox* LB=new ListBox;
		DSS[i]=LB;
		LB->NeedToDraw=true;
		LB->Parent=Parent;
		LB->AFont=AFont;
		LB->PFont=PFont;
		LB->ItemPic=ItemPic;
		LB->x=x+BaseX;
		LB->y=y+BaseY;
		LB->oneLy=ItemPic->PicPtr[1];
		LB->oneLx=ItemPic->PicPtr[0];
		LB->NItems=0;
		LB->FirstItem=NULL;
		LB->LastItem=NULL;
		LB->y1=LB->y+Ny*int(LB->oneLy);
		LB->x1=LB->x+LB->oneLx;
		LB->OnDraw=&ListBox_OnDraw;
		LB->OnMouseOver=&ListBox_OnMouseOver;
		LB->Refresh=&ListBox_Refresh;
		LB->FLItem=0;
		LB->CurItem=0;
		LB->ny=Ny;
		LB->VS=VS;
		return LB;
	};
	return NULL;
};
//-------------end of List Box---------------//
//-----------class : InputBox----------------//
bool InputBox_OnDraw(SimpleDialog* SD){
	InputBox* IB=(InputBox*)SD;
	if(SD){
		char base[128];
		strcpy(base,IB->Str);
		if(IB->Active)strcat(base,"_");
		IB->Pict->Draw(IB->x,IB->y);
		if(SD->Active)ShowString(IB->x+8,IB->y+4,base,IB->AFont);
		else ShowString(IB->x+8,IB->y+4,base,IB->Font);
	};
	return true;
};
bool InputBox_OnClick(SimpleDialog* SD){
	if(!SD->Active)SD->NeedToDraw=true;
	SD->Active=true;
	return true;
};
bool InputBox_OnKeyDown(SimpleDialog* SD){
	SD->NeedToDraw=true;
	InputBox* IB=(InputBox*)SD;
	if(LastKey==VK_BACK){
		if(strlen(IB->Str)>0)IB->Str[strlen(IB->Str)-1]=0;
		return true;
	}else{
		if(LastKey==' '||(LastKey>='0'&&LastKey<='9')||(LastKey>='A'&&LastKey<='Z')){
			char xx[2];
			xx[1]=0;
			xx[0]=char(LastKey);
			if(strlen(IB->Str)<IB->StrMaxLen)strcat(IB->Str,xx);
			return true;
		};
	};
	return false;
};
InputBox* DialogsSystem::addInputBox(SimpleDialog* Parent,
									 int x,int y,char* str,
									 int Len,
									 SQPicture* Panel,
									 RLCFont* RFont,
									 RLCFont* AFont){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		InputBox* IB=new InputBox;
		DSS[i]=IB;
		IB->NeedToDraw=true;
		IB->Parent=Parent;
		IB->x=x+BaseX;
		IB->y=y+BaseY;
		IB->Pict=Panel;
		IB->Font=RFont;
		IB->AFont=AFont;
		IB->StrMaxLen=Len;
		IB->Str=str;
		IB->x1=IB->x+Panel->PicPtr[0]-1;
		IB->y1=IB->y+Panel->PicPtr[1]-1;
		IB->OnDraw=&InputBox_OnDraw;
		IB->OnClick=&InputBox_OnClick;
		IB->OnKeyDown=&InputBox_OnKeyDown;
		return IB;
	};
	return NULL;
};
//------end of class InputBox----------------//
//----------class : CheckBox-----------------//
bool CheckBox_OnDraw(SimpleDialog* SD){
	CheckBox* CB=(CheckBox*)SD;
	if(CB->State)CB->OnPic->DrawTransparent(CB->x,CB->y);
	else CB->OffPic->DrawTransparent(CB->x,CB->y);
	if(CB->MouseOver)ShowString(CB->x+CB->OnPic->PicPtr[0],CB->y+4,CB->Message,CB->Font);
	else ShowString(CB->x+CB->OnPic->PicPtr[0],CB->y+4,CB->Message,CB->AFont);
	return true;
};
bool CheckBox_OnClick(SimpleDialog* SD){
	SD->NeedToDraw=true;
	CheckBox* CB=(CheckBox*)SD;
	if(CB->GroupIndex){
		DialogsSystem* DSM=CB->DS;
		for(int i=0;i<512;i++){
			CheckBox* SDD=(CheckBox*)DSM->DSS[i];
			if(SDD&&SDD->OnClick==&CheckBox_OnClick&&SDD->GroupIndex==CB->GroupIndex){
				SDD->State=false;
				SDD->NeedToDraw=true;
			};
		};
		CB->State=true;
	}else{
		CB->State=!CB->State;
	};
	return false;
};
bool CheckBox_Destroy(SimpleDialog* SD){
	CheckBox* CB=(CheckBox*)SD;
	free(CB->Message);
	return true;
};
bool CheckBox_OnMouseOver(SimpleDialog* SD){
	SD->NeedToDraw=true;
	return false;
};
CheckBox* DialogsSystem::addCheckBox(SimpleDialog* Parent,
									 int x,int y,char* message,
									 int group,bool State,
									 SQPicture* OnPict,
									 SQPicture* OffPict,
									 RLCFont* Font,
									 RLCFont* AFont){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		CheckBox* CB=new CheckBox;
		DSS[i]=CB;
		CB->MouseOverActive=true;
		CB->NeedToDraw=true;
		CB->Parent=Parent;
		CB->x=x+BaseX;
		CB->y=y+BaseY;
		CB->GroupIndex=group;
		CB->State=State;
		CB->OnPic=OnPict;
		CB->OffPic=OffPict;
		CB->x1=CB->x+OnPict->PicPtr[0]+GetRLen(message,Font)-1+2;
		CB->y1=CB->y+OnPict->PicPtr[1]-1+2;
		CB->DS=this;
		CB->OnClick=&CheckBox_OnClick;
		CB->Destroy=&CheckBox_Destroy;
		CB->OnDraw=&CheckBox_OnDraw;
		CB->Font=Font;
		CB->AFont=AFont;
		CB->Message=new char[strlen(message)+1];
		strcpy(CB->Message,message);
		return CB;
	};
	return NULL;
};
//--------end of CheckBox--------------------//
//-----------colored bar---------------------//
void CBar(int x,int y,int Lx,int Ly,byte c){
	if(Lx<=0||Ly<=0)return;
	int ofst=int(ScreenPtr)+x+y*SCRSizeX;
	int adds=SCRSizeX-Lx;
	int Lx4=Lx>>2;
	int Lx1=Lx&3;
	__asm{
		push	edi
		mov		edi,ofst
		mov		edx,Ly
		cld
		mov		al,c
		mov		ah,al
		shl		eax,16
		mov		al,c
		mov		ah,al
		mov		ebx,Lx1
qwr:	mov		ecx,Lx4
		rep		stosd
		mov		ecx,ebx
		rep		stosb
		add		edi,adds
		dec		edx
		jnz		qwr
		pop		edi
	};
};
bool ColoredBar_OnDraw(SimpleDialog* SD){
	byte c=((ColoredBar*)SD)->color;
	CBar(SD->x,SD->y,SD->x1-SD->x+1,SD->y1-SD->y+1,c);
	return true;
};
ColoredBar* DialogsSystem::addColoredBar(int x,int y,int Lx,int Ly,byte c){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		ColoredBar* PIC=new ColoredBar;
		DSS[i]=PIC;
		PIC->color=c;
		PIC->x=x+BaseX;
		PIC->y=y+BaseY;
		PIC->x1=PIC->x+Lx;
		PIC->y1=PIC->y+Ly;
		PIC->Parent=NULL;
		PIC->OnDraw=&ColoredBar_OnDraw;
		return PIC;
	};
	return NULL;
};
//---------------Text Viewer-----------------//
//---------Service functions---------

void TextViewer::GetNextLine(LineInfo* inf){
	int offs=inf->Offset;
	int x=0;
	word LS=0;
	word LW=0; 
	word NS=0;
	//char tms[128];
	char c;
	int tmsp=0;
	int ofs;
	int wid;
	int spw=GetRLCWidth(Font->RLC,' ');
	int nlsp=0;//кол-во пробелов перед словом
	while(offs<TextSize){
		//char c=TextPtr[offs];
		//if(c==' '){
			//reading next word
			tmsp=0;//length of string
			nlsp=0;//amount of spaces
			wid=0;
			while(TextPtr[offs+nlsp]==' '&&offs+nlsp<TextSize){
				nlsp++;
				wid+=spw;
			};
			do{
				ofs=offs+nlsp+tmsp;
				c=0;
				if(ofs<TextSize){
					c=TextPtr[ofs];
					if(c!=' '&&c!=0x0D){
						//tms[tmsp]=c;
						tmsp++;
						wid+=GetRLCWidth(Font->RLC,c);
					};
				};
			}while(ofs<TextSize&&c!=' '&&c!=0x0D);
			if(LW+wid<Lx&&c==0x0D){
				inf->NeedFormat=false;
				inf->NextOffset=offs+tmsp+nlsp+2;
				inf->NSpaces=NS;
				inf->LineWidth=LW;
				inf->LineSize=offs-inf->Offset+tmsp+nlsp;
				if(inf->NextOffset>TextSize)inf->NextOffset=-1;
				return;
			};
			if(LW+wid<Lx){
				//still current line
				LW+=wid;
				offs+=nlsp+tmsp;
				NS+=nlsp;
			}else{
				//next line
			/*	if(c!=' '){
					inf->NeedFormat=false;
					inf->NextOffset=offs;
					//if(c==0x0D)inf->NextOffset+=2;
					inf->NSpaces=NS;
					inf->LineWidth=LW;
					inf->LineSize=offs-inf->Offset;
					if(inf->NextOffset>TextSize)inf->NextOffset=-1;
					return;
				}else{*/
				inf->NeedFormat=true;
				inf->NextOffset=offs+nlsp;
				inf->NSpaces=NS;
				inf->LineWidth=LW;
				inf->LineSize=offs-inf->Offset;
				if(inf->NextOffset>TextSize)inf->NextOffset=-1;
				return;
				//};
			};
		//};
	};
	inf->NeedFormat=false;
	inf->NextOffset=-1;
	inf->NSpaces=NS;
	inf->LineWidth=LW;
	inf->LineSize=offs-inf->Offset;
};
void TextViewer::CreateLinesList(){
	if(!TextPtr)return;
	NLines=0;
	LineInfo LINF;
	LINF.Offset=0;
	do{
		GetNextLine(&LINF);
		if(LINF.NextOffset!=-1)NLines++;
		LINF.Offset=LINF.NextOffset;
	}while(LINF.Offset!=-1);
	LinePtrs=new char* [NLines];
	LineSize=new word [NLines];
	LineWidth=new word [NLines];
	NeedFormat=new bool [NLines];
	NSpaces=new word [NLines];
	LINF.Offset=0;
	NLines=0;
	do{
		GetNextLine(&LINF);
		if(LINF.NextOffset!=-1){
			LinePtrs[NLines]=TextPtr+LINF.Offset;
			LineSize[NLines]=LINF.LineSize;
			LineWidth[NLines]=LINF.LineWidth;
			NeedFormat[NLines]=LINF.NeedFormat;
			NSpaces[NLines]=LINF.NSpaces;
			NLines++;
		};
		LINF.Offset=LINF.NextOffset;
	}while(LINF.Offset!=-1);
};
bool TextViewer_OnDestroy(SimpleDialog* SD){
	TextViewer* TV=(TextViewer*)SD;
	if(TV->TextPtr){
		free(TV->TextPtr);
		free(TV->LinePtrs);
		free(TV->LineSize);
		free(TV->LineWidth);
		free(TV->NeedFormat);
		free(TV->NSpaces);
		TV->TextPtr=NULL;
	};
	return true;
};
bool TextViewer_OnDraw(SimpleDialog* SD){
	TextViewer* TV=(TextViewer*)SD;
	byte c;
	if(!TV->TextPtr)return false;
	if(TV->Line>TV->NLines)TV->Line=TV->NLines-TV->PageSize+1;
	if(TV->Line<0)TV->Line=0;
	for(int i=0;i<TV->PageSize;i++){
		int lin=TV->Line+i;
		int yy=TV->y+TV->SymSize*i;
		if(lin<TV->NLines){
			byte* off=(byte*)(TV->LinePtrs[lin]);
			int x=0;
			int LS=TV->LineSize[lin];
			int LW=TV->LineWidth[lin];
			int NS=TV->NSpaces[lin];
			int addsp=0;
			if(NS>0)addsp=div(TV->Lx-LW,NS).quot;
			if(TV->NeedFormat[lin]){
				for(int j=0;j<LS;j++){
					c=off[j];
					ShowRLCItem(TV->x+x,yy,&TV->Font->RLC,c,0);
					x+=GetRLCWidth(TV->Font->RLC,c);
					if(c==' ')x+=addsp;
				};
			}else{
				for(int j=0;j<LS;j++){
					c=off[j];
					ShowRLCItem(TV->x+x,yy,&TV->Font->RLC,c,0);
					x+=GetRLCWidth(TV->Font->RLC,c);
				};
			};
		};
	};
	return true;
};
bool TextView_OnKeyDown(SimpleDialog* SD){
	TextViewer* TV=(TextViewer*)SD;
	if(!TV->TextPtr)return false;
	if(KeyPressed){
		switch(LastKey){
		case 38:
			TV->Line--;
			KeyPressed=false;
			return true;
		case 40:
			TV->Line++;
			KeyPressed=false;
			return true;
		case 33:
			TV->Line-=TV->PageSize-1;
			KeyPressed=false;
			return true;
		case 34:
			TV->Line+=TV->PageSize-1;
			KeyPressed=false;
			return true;
		};
	};
	return false;
};
TextViewer* DialogsSystem::addTextViewer(SimpleDialog* Parent,int x,int y,int Lx,int Ly,char* TextFile,RLCFont* Font){
	for(int i=0;i<512&&DSS[i]!=NULL;i++);
	if(i<512){
		TextViewer* TV=new TextViewer;
		DSS[i]=TV;
		TV->NeedToDraw=true;
		TV->Parent=Parent;
		TV->x=x+BaseX;
		TV->y=y+BaseY;
		TV->x1=TV->x+Lx-1;
		TV->y1=TV->y+Ly-1;
		TV->Lx=Lx;
		TV->OnDraw=&TextViewer_OnDraw;
		TV->Destroy=&TextViewer_OnDestroy;
		TV->Font=Font;
		TV->Line=0;
		TV->SymSize=GetRLCHeight(Font->RLC,'W')+6;
		TV->PageSize=div(Ly,TV->SymSize).quot;
		ResFile f1=RReset(TextFile);
		if(IOresult()){
			TV->TextPtr=NULL;
			TV->NLines=0;
			return TV;
		};
		TV->TextSize=RFileSize(f1);
		TV->TextPtr=new char [TV->TextSize+32];
		RBlockRead(f1,TV->TextPtr,TV->TextSize);
		RClose(f1);
		TV->CreateLinesList();
		TV->OnKeyDown=&TextView_OnKeyDown;
		TV->Enabled=true;
		TV->Active=true;
		return TV;
	};
	return NULL;
}; 
//-------------------------------------------//
SimpleDialog::SimpleDialog(){
	Enabled=false;
	x=0;
	y=0;
	x1=0;
	y1=0;
	UserParam=0;
	OnClick=NULL;
	OnUserClick=NULL;
	OnDraw=NULL;
	OnActivate=NULL;
	OnKeyDown=NULL;
	OnLeave=NULL;
	OnMouseOver=NULL;
	Destroy=NULL;
	Refresh=NULL;
	Parent=NULL;
	MouseOver=false;
	MouseOverActive=false;
	Active=false;
	Enabled=true;
};
//----copy rectangle to screen----//
void CopyToScreen(int zx,int zy,int zLx,int zLy){
	int x=zx;
	int y=zy;
	int Lx=zLx;
	int Ly=zLy;
	//if(zLx<=0||zLy<=0||zx<0||zy<0||zx+zLx>RealLx||zy+zLy>RealLy)return;
	//if(zLx<=0||zLy<=0||zx<0||zy<0)return;
	if(x<0){
		Lx+=x;
		x=0;
	};
	if(y<0){
		Ly+=y;
		y=0;
	};
	if(x+Lx>RealLx)Lx=RealLx-x;
	if(y+Ly>RealLy)Ly=RealLy-y;
	if(Lx<0||Ly<0)
		return;
	int scof=int(ScreenPtr)+x+y*SCRSizeX;
	int reof=int(RealScreenPtr)+x+y*RSCRSizeX;
	int sadd=SCRSizeX-Lx;
	int radd=RSCRSizeX-Lx;
	int Lx4=Lx>>2;
	int Lx1=Lx&3;
	__asm{
		push	esi
		push	edi
		mov		edx,Ly
		or		edx,edx
		jz		lpp4
		mov		esi,scof
		mov		edi,reof
		cld
lpp1:	mov		ecx,Lx4
		jcxz	lpp2
		rep		movsd
lpp2:	mov		ecx,Lx1
		jcxz	lpp3
		rep		movsb
lpp3:	add		esi,sadd
		add		edi,radd
		dec		edx
		jnz		lpp1
lpp4:	pop		edi
		pop		esi
	};
};
//--------Pictures methods--------//
void SQPicture::Draw(int x,int y){
	if(!PicPtr)return;
	int sofst=int(ScreenPtr)+x+y*SCRSizeX;
	int pofst=int(PicPtr+2);
	int lx=int(PicPtr[0]);
	int ly=int(PicPtr[1]);
	if(ly<=0||lx<=0||lx>RealLx||ly>RealLy)return;
	int lx4=lx>>2;
	int lx1=lx&3;
	int scradd=SCRSizeX-lx;
	__asm{
		push	esi
		push	edi
		mov		esi,pofst
		mov		edi,sofst
		mov		edx,ly
		or		edx,edx
		jz		uu3
		cld		
lpp1:	mov		ecx,lx4
		jcxz	uu1
		rep		movsd
uu1:	mov		ecx,lx1
		jcxz	uu2
		rep		movsb
uu2:	add		edi,scradd
		dec		edx
		jnz		lpp1
uu3:	pop		edi
		pop		esi
	};
};
void SQPicture::DrawTransparent(int x,int y){
	if(!PicPtr)return;
	int sofst=int(ScreenPtr)+x+y*SCRSizeX;
	int pofst=int(PicPtr+2);
	int lx=int(PicPtr[0]);
	int ly=int(PicPtr[1]);
	if(ly<=0||lx<=0||lx>RealLx||ly>RealLy)return;
	int lx4=lx>>2;
	int lx1=lx&3;
	int scradd=SCRSizeX-lx;
	__asm{
		push	esi
		push	edi
		mov		esi,pofst
		mov		edi,sofst
		mov		edx,ly
		or		edx,edx
		jz		uu3
		cld		
lpp0:	mov		ecx,lx
lpp1:	lodsb
		or		al,al
		jnz		lpp1x
		//inc		esi
		inc		edi
		dec		ecx
		jnz		lpp1
		jmp		uu2
lpp1x:	stosb
		dec		ecx
		jnz		lpp1
uu2:	add		edi,scradd
		dec		edx
		jnz		lpp0
uu3:	pop		edi
		pop		esi
	};
};
void SQPicture::LoadPicture(char* name){
	ResFile ff1=RReset(name);
	if(!IOresult()){
		int Lx=0;
		int Ly=0;
		RBlockRead(ff1,&Lx,2);
		RBlockRead(ff1,&Ly,2);
		if(Lx>0&&Ly>0){
			PicPtr=(word*)(new char[Lx*Ly+4]);
			RBlockRead(ff1,PicPtr+2,Lx*Ly);
			PicPtr[0]=Lx;
			PicPtr[1]=Ly;
		}else PicPtr=NULL;
	}else{
		PicPtr=NULL;
		
	};
};
SQPicture::~SQPicture(){
	if(PicPtr)free(PicPtr);
};
SQPicture::SQPicture(){
	PicPtr=NULL;
};
//-----end of pictures
//--------------Handling dialog system----------------//
void MFix();
void RedrawGameBackground();
void DialogsSystem::ProcessDialogs(){
	if(MUSTDRAW){
		MUSTDRAW=false;
		RedrawGameBackground();
		MarkToDraw();
	};
	for(int i=0;i<512;i++){
		SimpleDialog* SD=DSS[i];
		if(SD&&SD->Enabled){
			if(SD->Refresh)
				SD->Refresh(SD);
			if(SD->Active){
				if(SD->OnKeyDown){
					if(KeyPressed&&SD->OnKeyDown(SD))KeyPressed=false;
				};
			};
			if(mouseX>=SD->x&&mouseX<=SD->x1&&mouseY>=SD->y&&mouseY<=SD->y1){
				if(SD->OnMouseOver)SD->OnMouseOver(SD);
				if(SD->MouseOverActive)SD->NeedToDraw=true;
				SD->MouseOver=true;
				if(Lpressed){
					if(SD->OnUserClick){
						SD->OnUserClick(SD);
						Lpressed=false;
					};
					if(SD->OnClick){
						if(SD->OnClick(SD)){
							for(int j=0;j<512;j++){
								SimpleDialog* SD1=DSS[j];
								if(SD1&&SD1!=SD)SD1->Active=false;
							};
						};
						Lpressed=false;
					};
				};
			}else{
				if(SD->MouseOver&&SD->MouseOverActive)SD->NeedToDraw=true;
				SD->MouseOver=false;
			};
		};
		if(SD&&!SD->Enabled){
			if(SD->OnDraw)SD->OnDraw(SD);
			SD->NeedToDraw=true;
		};
	};
	for(i=0;i<512;i++){
		SimpleDialog* SD=DSS[i];
		if(SD&&SD->OnDraw){
			if(SD->NeedToDraw||(SD->Parent&&SD->Parent->NeedToDraw)){
				//if(SD->Parent&&SD->Parent->NeedToDraw)SD->NeedToDraw=true;
				SD->OnDraw(SD);
			};
		};
	};
	MFix();
};
void RedrawOffScreenMouse();
void OnMouseMoveRedraw();
void PostRedrawMouse();
void SERROR();
void SERROR1();
void SERROR2();
extern bool InGame;
void DialogsSystem::RefreshView(){
	if(!DDDebug){
		//if(InGame)SERROR1();
		RedrawOffScreenMouse();
		//if(InGame)SERROR1();
		//FlipPages();
		for(int i=0;i<512;i++){
			//if(InGame)SERROR();
			SimpleDialog* SD=DSS[i];
			if(SD&&SD->NeedToDraw){
				CopyToScreen(SD->x,SD->y,SD->x1-SD->x+1,SD->y1-SD->y+1);
				SD->NeedToDraw=false;
			};
		};
		//if(InGame)SERROR2();
		PostRedrawMouse();
		//if(InGame)SERROR1();
	}else{
		if(DDDebug)RSCRSizeX=SCRSizeX;
		RedrawOffScreenMouse();
		PostRedrawMouse();
		//LockMouse=false;
		FlipPages();
	};
};
void DialogsSystem::CloseDialogs(){
	for(int i=0;i<512;i++){
		SimpleDialog* SD=DSS[i];
		if(SD){
			if(SD->Destroy)SD->Destroy(SD);
			free(SD);
			DSS[i]=NULL;
		};
	};
};
void DialogsSystem::MarkToDraw(){
	for(int i=0;i<512;i++)
		if(DSS[i])DSS[i]->NeedToDraw=true;
};
//----------end of handler of the dialogsystem--------//
DialogsSystem::~DialogsSystem(){
	CloseDialogs();
};