call reg_dll()
call get_code()

sub reg_dll()
	set ws=createobject("Wscript.Shell")
	ws.run "regsvr32 dm.dll /s"
	set ws=nothing
	wscript.sleep 1500 
end sub

sub get_code()
	set dm = createobject("dm.dmsoft")
	if check_ver(dm.ver()) = 0 then
		msgbox "����汾̫�ϣ�����ʹ��2.1144֮��İ汾����ǰ����汾����:"&dm.ver()
		exit sub
	end if
	
	machine_code = dm.GetMachineCode()
	dm.SetClipBoard machine_code
	msgbox "���Ļ�������:"&machine_code&",�Ѿ����Ƶ���������"
end sub

function check_ver(byval ver)
	check_ver = 0
	
	if len(ver) = 0 then exit function
	
	ver_array = split(ver,".")
	if ubound(ver_array) <> 1  then exit function
	
	if cint(ver_array(0)) > 2 then
		check_ver = 1
		exit function
	end if
	
	if cint(ver_array(0)) < 2 then
		exit function
	end if
	
	tmp = ""
	for   i=1   to   len(ver_array(1)) 
        if asc(mid(ver_array(1),i,1)) >= 48   and   asc(mid(ver_array(1),i,1)) <= 57   then 
              tmp = tmp & mid(ver_array(1),i,1) 
        end   if 
	next 
	
	if cint(tmp) >= 1144 then
		check_ver = 1
		exit function
	end if
end function