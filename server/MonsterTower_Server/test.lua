
my_id = "no"
my_password = "nono"
my_scene = "nono"

my_info = {}

function SetMyInfo(id, password, scene)
	my_id = id;
	my_password = password;
	my_scene = scene;
end

function ShowJob()
	for m_key, m_value in my_info do
		print(m_key, " , ", m_value);
	end
end

function showAll ()
	print("ID : \t", my_id, "\t Password : ", my_password);
	local temp_exp = math.floor(my_info["exp"]*100)/100;
	print(my_info["job"], my_info["level"], temp_exp, "\n", my_info["hp"], my_info["mp"], my_info["damage"], my_info["def"]);
	print("------");
end
