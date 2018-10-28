
job_list = {}
job_list["전사"] = {level = 1, exp = 0.1, hp = 100, mp = 10, damage = 60, def = 100}
job_list["궁수"] = {level = 1, exp = 0.1, hp = 60, mp = 50, damage = 80, def = 60}
job_list["마법사"] = {level = 1, exp = 0.1, hp = 50, mp = 100, damage = 100, def = 40}


function DataLoad(strPath)
	local file = io.open(strPath, "rb");
	local data;

	if file then
		data = file:read("*all");
		file:close();
		--print(data);
	end

	return data;
end


function DataSave(strPath, strData)
	local file = io.open(strPath, "wb");
	
	if file then
		file:write(strData);
	end
end


function SelectCharacter()
	local num = API_CharacterList(job_list);

	return job_list[num].def, job_list[num].damage, job_list[num].mp, job_list[num].hp, job_list[num].exp, job_list[num].level, num
end
