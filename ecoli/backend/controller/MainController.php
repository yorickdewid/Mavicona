<?php

use Ecoli\Response;

class MainController
{
	/**
	 * Retrieve all packages.
	 *
	 * @return Ecoli\Response
	 */
	public function getIndex()
	{
		$content = "<h1>Welcome</h1>";
		$content .= "<i>Ecoli</i>";

		return new Response($content);
	}

}