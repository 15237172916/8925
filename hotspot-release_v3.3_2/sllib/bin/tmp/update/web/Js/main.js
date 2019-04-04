var hostip =window.location.host;

var txName = ['','','','','','','','','','','','','','','','','','','','','','','',''];
var rxName = ['','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','',''];
//var rxMulticast = ['','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','','',''];
//var rxMulticast = ['001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001','001'];
var rxMulticast = [1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1];
var modeName = ['', '', '', '', '','', '', '', '', ''];

var num = ['001','002','003','004','005','006','007','008','009','010','011','012','013','014','015','016','017','018','019','020','021','022','023','024','025','026','027','028','029','030','031','032','033','034','035','036','037','038','039','040','041','042','043','044','045','046','047','048','049','050','051','052','053','054','055','056','057','058','059','060','061','062','063','064','065','066','067','068','069','070','071','072','073','074','075','076','077','078','079','080','081','082','083','084','085','086','087','088','089','090','091','092','093','094','095','096','097','098','099','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','121','122','123','124','125','126','127','128','129'];
var num1=['01','02','03','04','05','06','07','08','09','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24'];
var num2=['1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','121','122','123','124','125','126','127','128','129'];

var currentMode;


function changeModeName(number, name)
{
	var orderNumber = Number(number);
	modeName[orderNumber] = name;
	var ret;
	var urlstr = "http://"+hostip+"/cgi-bin/changeModeName.cgi?Mode:"+orderNumber+"Name="+modeName[orderNumber]+"&";
	$.ajax({ 
		type:"GET",
		url: urlstr,
		dataType:"text",
		cache:false,
		async:false,	//true,
		success:function(data)
		{
			if(data=="succeed")
			{
				//console.log(data);
				ret=1;
				//alert('Setting Successful!');	
			}
			else
			{
				alert('Setting Failed!');	
				ret = 0;
			}
		}
	});
	//console.log(name);
	//alert(rxName[orderNumber]);
	return ret;
}

function changeRxName(number, name)
{
	var orderNumber = Number(number);
	rxName[orderNumber] = name;
	
	var ret;
	var urlstr = "http://"+hostip+"/cgi-bin/changeRxName.cgi?Rx:"+orderNumber+"Name="+rxName[orderNumber]+"&";
	$.ajax({ 
		type:"GET",
		url: urlstr,
		dataType:"text",
		cache:false,
		async:false,	//true,
		success:function(data)
		{
			if(data=="succeed")
			{
				//console.log(data);
				ret=1;
			}
			else
			{
				alert('Setting Failed!');	
				ret = 0;
			}
		}
	});
	//console.log(name);
	//alert(rxName[orderNumber]);
	return ret;
}

function changeTxName(number, name)
{
	var orderNumber = Number(number);
	txName[orderNumber] = name;
	var ret;
	var urlstr = "http://"+hostip+"/cgi-bin/changeTxName.cgi?Rx:"+orderNumber+"Name="+txName[orderNumber]+"&";
	$.ajax({ 
		type:"GET",
		url: urlstr,
		dataType:"text",
		timeout:2000,
		cache:false,
		async:false,	//true,
		success:function(data)
		{
			if(data=="succeed")
			{
				//console.log(data);
				ret=1;
			}
			else
			{
				alert('Setting Failed!');	
				ret = 0;
			}
		}
	});
	//console.log(name);
	//alert(rxName[orderNumber]);
	return ret;
}

function changeMulticast(multicast, number)
{
	var multicastNum = Number(multicast);
	var orderNumber = Number(number);
	rxMulticast[orderNumber] = multicastNum;
	
	var ret;
	var urlstr = "http://"+hostip+"/cgi-bin/changeMulticast.cgi?Multicast:"+orderNumber+"Name="+rxMulticast[orderNumber]+"&";
	$.ajax({ 
		type:"GET",
		url: urlstr,
		dataType:"text",
		timeout:2000,
		cache:false,
		async:false,	//true,
		success:function(data)
		{
			if(data=="succeed")
			{
				//console.log(data);
				ret=1;
			}
			else
			{
				alert('Setting Failed!');	
				ret = 0;
			}
		}
	});
	//console.log(name);
	//alert(rxName[orderNumber]);
	return ret;
}

function saveCurrentMode(mode)
{
	var number = mode.slice(4,6);
	var ret;
	
	var urlstr = "http://"+hostip+"/cgi-bin/saveCurrentMode.cgi?saveMode:"+number+"&";
	$.ajax({ 
		type:"GET",
		url: urlstr,
		dataType:"text",
		timeout:200,
		cache:false,
		async:false,	//true,
		success:function(data)
		{
			if(data=="succeed")
			{
				//console.log(data);
				ret=1;
			}
			else
			{
				alert('Setting Failed!');	
				ret = 0;
			}
		}
	});
	//console.log(name);
	
	return ret;
}
function infoDisplay(num)
{
	var number=num;
	
	var urlstr = "http://"+hostip+"/cgi-bin/infoDisplay.cgi?info:"+number+"&";
	$.ajax({ 
		type:"GET",
		url: urlstr,
		dataType:"text",
		timeout:200,
		cache:false,
		async:false,	//true,
		success:function(data)
		{
			if(data=="succeed")
			{
				//console.log(data);
				ret=1;
			}
			else
			{
				alert('Setting Failed!');	
				ret = 0;
			}
		}
	});
	return ret;
}

function modeStateNow(mode)
{
	var number = mode.slice(4,6);
	var orderNumber = Number(number);
	
	window.parent.currentMode = orderNumber;
	//alert(number);
	//console.log(orderNumber);
	//return 1;
	
	//var urlstr = "http://"+hostip+"/cgi-bin/modeStateNow.cgi?currentMode:"+number+"&";
	var urlstr = "http://"+hostip+"/cgi-bin/modeStateNow.cgi?currentMode:"+number+"&";
	$.ajax({ 
		type:"GET",
		url: urlstr,
		dataType:"text",
		timeout:200,
		cache:false,
		async:false,	//true,
		success:function(data)
		{
			if(data=="succeed")
			{
				//console.log(data);
				ret=1;
			}
			else
			{
				alert('Setting Failed!');	
				ret = 0;
			}
		}
	});
	//console.log(name);
	//alert(rxName[orderNumber]);
	return ret;
}

function onloading()
{
	var rxElement;
	var txElement;
	var modeElement;
	var multicastElement;
	var currentElement;
	
	var i;

	for (i=0; i<128; i++)
	{
		rxName[i] = "TV-"+num[i];
		//console.log(rxName[i]);
	}
	//alert(rxName);
	for (i=0; i<24; i++)
	{
		txName[i] = "SOURCE-"+num1[i];
		//console.log(txName[i]);
	}
	//alert(txName);	
	for (i=0; i<10; i++)
	{
		
		modeName[i] = "MODE-"+num1[i];
		//console.log(modeName[i]);
	}
	currentMode = 1;
	//alert(modeName);
	//alert("");
	
	$.ajax({
		type: "GET",
		url: "http://" + hostip + "/cgi-bin/onloading.cgi",
		dataType:"xml",
		cache: false,
		async:false,
		success: function(data) {
			for (i=0; i<128; i++)
			{
				rxElement= "RX"+num2[i];//i.toString;
				rxName[i] = $(data).find(rxElement).text();
				//console.log(rxName[i]);
				//$("#input_type").append(input_type);
				//alert(rxName);
			}
			for (i=0; i<128; i++)
			{
				multicastElement = "MUL"+num2[i];//i.toString;
				rxMulticast[i] = $(data).find(multicastElement).text();
				//console.log(rxMulticast[i]);
				//$("#input_type").append(input_type);
			}
			for (i=0; i<24; i++)
			{
				txElement = "TX"+num2[i];//i.toString;
				//console.log(txElement);
				txName[i] = $(data).find(txElement).text();
			}
			for (i=0; i<10; i++)
			{
				modeElement = "MODE"+num2[i];//i.toString;
				modeName[i] = $(data).find(modeElement).text();
			}
			currentElement = "CurrentMode";
			currentMode = $(data).find(currentElement).text();
			//console.log(currentMode);
		}
	});
}

function SelectButtonNameInit()
{
	var SelectButtonId;
	var RxNameId;
	var rxName, txName;

	for (var i=0; i<128; i++)
	{
		RxNameId = "rxName"+num[i];
		SelectButtonId = "button"+num[i];
		multicast = window.parent.rxMulticast[i]-1;
		rxName = window.parent.rxName[i];
		txName = window.parent.txName[multicast];
		//console.log(multicast);
		if (""==txName)
		{
			document.getElementById(SelectButtonId).value = "SOURCE-"+num1[multicast];
		}
		else
		{
			document.getElementById(SelectButtonId).value = window.parent.rxMulticast[i]+'-'+txName;
		}
		//document.getElementById(SelectButtonId).value = window.parent.txName[multicast];
		if (""==rxName)
		{
			document.getElementById(RxNameId).value = "TV-"+num[i];
		}
		else
		{
			document.getElementById(RxNameId).value = num[i]+'-'+rxName;
		}
		
	}
	//console.log(window.parent.txName);
	//alert(window.parent.txName);
}

function TxButtonNameInit()
{
	var TxNameId;
	var name;

	for (var i=0; i<24; i++)
	{
		TxNameId = "TX"+num1[i];
		name = window.parent.txName[i];

		if (""==name)
		{
			document.getElementById(TxNameId).value = "SOURCE-"+num1[i];
		}
		else
		{
			document.getElementById(TxNameId).value = num[i]+'-'+name;
		}
	}
}

function ModeInit()
{
	var modeButtonId;
	var currentModeNum;
	var name;

	for (var i=0; i<10; i++)
	{
		modeButtonId = "Mode"+num1[i];
		name = window.parent.modeName[i];

		if (""==name)
		{
			document.getElementById(modeButtonId).value = "MODE-"+num1[i];
		}
		else
		{
			document.getElementById(modeButtonId).value = name;
		}
	}
	
	currentModeNum=Number(window.parent.currentMode)-1;
	modeButtonId = "Mode"+num1[currentModeNum];
	document.getElementById(modeButtonId).style.backgroundColor = "#006000";
}















